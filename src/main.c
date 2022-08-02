/*
 * Copyright (c) 2006-2020, DreamGrow Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-07-07     luhuadong    the first version
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include "agloo.h"

#define AG_GLOBALS
#include "fsm.h"
#include "smm.h"

AG_EXT State context;
static msg_node_t client;

#define APPLICATION_NAME     "Workflow Controller"
#define DEFAULT_FILE         "/var/run/agloo.pid"

static pid_t s_pid;
static int lock_fd;

static void sigroutine(int signal)
{
    switch (signal)
    {
    case 1:
        printf("Get a signal -- SIGHUP\n"); // 1
        break;
    case 2:
        printf("Get a signal -- SIGINT\n"); // 2: 强制关闭（直接 kill 进程）
        smm_kill_all_modules();
        exit(0);
        break;
    case 3:
        printf("Get a signal -- SIGQUIT\n"); // 3: 正常关闭（通过消息关闭）
        smm_stop_all_modules();
        exit(0);
        break;
    }
    return;
}

static int create_lock_file(const char *lockfile)
{
    lock_fd = open(lockfile, O_WRONLY | O_CREAT, 0666);
    if (lock_fd < 0)
    {
        perror("Fail to open");
        return -AG_ENOFILE;
    }

    struct flock lock;

    bzero(&lock, sizeof(lock));

    if (fcntl(lock_fd, F_GETLK, &lock) < 0)
    {
        perror("Fail to fcntl F_GETLK");
        close(lock_fd);
        LOG_I("%s has been running", APPLICATION_NAME);
        return -AG_ENOPERM;
    }

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;

    if (fcntl(lock_fd, F_SETLK, &lock) < 0)
    {
        perror("Fail to fcntl F_SETLK");
        close(lock_fd);
        LOG_I("%s has been running", APPLICATION_NAME);
        return -AG_ENOPERM;
    }

    char buf[32] = {0};
    s_pid = getpid();

    int len = snprintf(buf, 32, "%d\n", (int)s_pid);
    write(lock_fd, buf, len); // Write pid to the file

    return AG_EOK;
}

static void destroy_lock_file(int fd)
{
    if (fd) {
        close(fd);
    }
}

static int get_instance_pid(const char *lockfile)
{
    lock_fd = open(lockfile, O_RDONLY, 0644);
    if (lock_fd < 0)
    {
        perror("Fail to open");
        return -AG_ENOFILE;
    }

    char buf[32] = {0};
    char cmd[64] = {0};
    int ret;

    ret = read(lock_fd, buf, sizeof(buf));
    if(ret == -1) {
        printf("read Error\n");
        close(lock_fd);
        return -AG_EIO;
    }

    LOG_I("PID: %s", buf);
    close(lock_fd);
    
    return atoi(buf);
}

static int startup_modules(void)
{
    /*
     * 3 method:
     * (1) system
     * (2) exec
     * (3) fork
     */
    system("/opt/agloo/bin/MessageDispatcher &");
    system("/opt/agloo/bin/MonitorAlarm &");
    system("/opt/agloo/bin/WebServer &");
    system("/opt/agloo/bin/OTA &");
    system("/opt/agloo/bin/ConfigManager &");
    system("/opt/agloo/bin/DeviceManager &");
    system("/opt/agloo/bin/Acquisition &");

    return AG_EOK;
}

static void _msg_arrived_cb(char *topic, void *payload, size_t payloadlen)
{
    if (0 == strncmp(SYS_EVENT_TOPIC, topic, strlen(topic))) {
        event_msg_t *e = (event_msg_t *)payload;
        printf("Payload len = %lu >> event: [%d] %s\n", payloadlen, e->event_id, e->event_data.event_str);

        if (AG_EVENT_EXCEPTION == e->event_id) {
            context.goWrong();
        }
        else if (AG_EVENT_RECOVERY == e->event_id) {
            context.recovery();
        }
    }
    else if (0 == strncmp(MOD_REGISTER_TOPIC, topic, strlen(topic)))
    {
        smm_msg_t *msg = (smm_msg_t *)payload;
        LOG_I("Register: %s, %d", msg->name, msg->pid);

        if (NULL == smm_register(msg->name, msg->class, msg->pid, NULL)) {
            LOG_E("Module %s register failed.", msg->name);
        }
    }
    
#if 0
    printf("[%s] %s\n", topic, (char *)payload);

    cJSON *json = cJSON_Parse(payload);
    printf("%s\n\n", cJSON_Print(json));
    cJSON_Delete(json);
#endif
}

static int msg_init(void)
{
    int rc = 0, cn = 0;

    rc = msg_bus_init(&client, "Workflow", NULL, _msg_arrived_cb);
    if (rc != AG_EOK) {
        printf("Message bus init failed.\n");
        return -1;
    }

    /* Subscription list */
    rc = msg_bus_subscribe(client, SYS_EVENT_TOPIC);
    if (rc != AG_EOK) cn++;
    rc = msg_bus_subscribe(client, MOD_REGISTER_TOPIC);
    if (rc != AG_EOK) cn++;

    if (cn != 0) {
        msg_bus_deinit(client);
        printf("Message bus subscribe failed.\n");
        return -AG_ERROR;
    }

    return AG_EOK;
}

int main(int argc, char *argv[])
{
    int rc;

    log_init("Workflow");
    sayHello(APPLICATION_NAME);

    printf("process id is %d\n", getpid());
    signal(SIGHUP, sigroutine);
    signal(SIGINT, sigroutine);
    signal(SIGQUIT, sigroutine);

    if (argc > 1)
    {
        if (0 == strncmp("stop", argv[1], strlen("stop")))
        {
            printf("** Stop %s\n", APPLICATION_NAME);

            rc = create_lock_file(DEFAULT_FILE);
            if (rc == AG_EOK) {
                LOG_I("Lock file was not locked up");
                destroy_lock_file(lock_fd);
                exit(1);
            }

            /* Send SIGQUIT signal */
            s_pid = get_instance_pid(DEFAULT_FILE);

            LOG_I("Send SIGQUIT signal to %d", s_pid);
            kill(s_pid, SIGQUIT);

            /* send stop signal */
            exit(1);
        }
    }

    rc = create_lock_file(DEFAULT_FILE);
    if (rc != AG_EOK) {
        LOG_I("Can not create lock file");
        exit(1);
    }

    fsm_init();
    msg_init();
    smm_init();

    context.init();

    startup_modules();

    context.sleep();
    context.init();  // invalid
    context.wakeUp();
    context.init();
    //context.powerOff();
    

    int count = 100;
    char buf[256] = {0};

    state_msg_t stateMsg = {0};

    while (1) {
        memset(&stateMsg, 0, sizeof(stateMsg));
        stateMsg.state_id = fsm_get_current_state();

        char *stateText = fsm_get_state_text(stateMsg.state_id);
        strncpy(stateMsg.state_str, stateText, strlen(stateText));

        msg_bus_publish_raw(client, SYS_STATE_TOPIC, (void *)&stateMsg, sizeof(stateMsg));

        fsm_sem_wait();
    }
    
    msg_bus_deinit(client);
    fsm_deinit();

    return 0;
}