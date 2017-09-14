#include <stdio.h>
#include <stdlib.h>
#include <sel4/sel4.h>
#include "sos.h"
#include "sys.h"
#include <string.h>
#include <assert.h>


pid_t sos_process_create(const char *path)
{
    // handle_no_implemented_syscall("sos_process_create");

	tty_debug_print("[app] sos_process_create with path %s\n", path);
    struct ipc_buffer_ctrl_msg ctrl_msg ;

    ctrl_msg.syscall_number = SOS_SYSCALL_CREATE_PROCESS;

    ctrl_msg.offset = strlen(path);

    if (ctrl_msg.offset >= APP_PROCESS_IPC_SHARED_BUFFER_SIZE)
    {
        ctrl_msg.offset = APP_PROCESS_IPC_SHARED_BUFFER_SIZE;
    }

    // currently, use file_id field to transfer proc_id
    ctrl_msg.file_id = -1;
    struct ipc_buffer_ctrl_msg ret;
    assert (0 == ipc_call(&ctrl_msg, path, &ret));
    tty_debug_print("[app] sos_process_create return %d, pid %d\n", ret.ret_val, ret.file_id);

    assert(ret.ret_val>=0);
    return (ret.ret_val == 0) ? ret.file_id: ( -ret.ret_val);
}

int sos_process_delete(pid_t pid)
{
	// It is the proc max, hard code for now
	if (pid <= 1 || pid >= 256) {
		tty_debug_print("[app] invalid pid for sos_process_delete\n");
		return -1;
	}

    tty_debug_print("[app] sos_process_delete with pid %d\n", pid);
    struct ipc_buffer_ctrl_msg ctrl_msg ;

    ctrl_msg.syscall_number = SOS_SYSCALL_PROCESS_DELETE;

    // currently, use file_id field to transfer proc_id
    ctrl_msg.file_id = pid;
    struct ipc_buffer_ctrl_msg ret;
    assert (0 == ipc_call(&ctrl_msg, NULL, &ret));
    tty_debug_print("[app] sos_process_delete return %d\n", ret.ret_val);

    assert(ret.ret_val == 0);
    // return (ret.ret_val == 0) ? ret.file_id: ( -ret.ret_val);
    return 0;
}


pid_t sos_process_wait(pid_t pid)
{
    handle_no_implemented_syscall("sos_process_wait");
    return 0;
}

int sos_process_status(sos_process_t *processes, unsigned max)
{
    handle_no_implemented_syscall("sos_process_status");
    return 0;
}


void sos_process_exit()
{
	tty_debug_print("[app] sos_process_exit start\n");

	/* should use seL4_Send, since we do not expect any response*/
    struct ipc_buffer_ctrl_msg ctrl_msg ;

    ctrl_msg.syscall_number = SOS_SYSCALL_PROCESS_EXIT;

    seL4_MessageInfo_t tag = seL4_MessageInfo_new(0, 0, 0, IPC_CTRL_MSG_LENGTH);
    seL4_SetTag(tag);

    serialize_ipc_ctrl_msg(&ctrl_msg);
    seL4_Send(SYSCALL_ENDPOINT_SLOT, tag);
}