/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright 2017-2020 SUSE LLC
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _VIRTIO_SP_COMMON_H
#define _VIRTIO_SP_COMMON_H

#define VIRTIO_SP_PHYS_CRASH_DUMP_SEGMENTS    8
#ifdef IS_STORPORT
#define VIRTIO_SP_MAX_SGL_ELEMENTS  16
#else
#define VIRTIO_SP_MAX_SGL_ELEMENTS  64
#endif

#define PVCTRL_QDEPTH_STR "qdepth"

#define SP_NUMBER_OF_ACCESS_RANGES PCI_TYPE0_ADDRESSES
#define SP_BUS_INTERFACE_TYPE PCIBus

#ifdef VBIF_DBG_TRACK_SRBS
extern uint32_t srbs_seen;
extern uint32_t srbs_returned;
extern uint32_t io_srbs_seen;
extern uint32_t io_srbs_returned;
extern uint32_t sio_srbs_seen;
extern uint32_t sio_srbs_returned;
#define VBIF_INC_SRB(_V)            InterlockedIncrement(&(_V))
#define VBIF_DEC_SRB(_V)            InterlockedDecrement(&(_V))
#ifdef VBIF_DBG_TRACK_AND_REPORT_SRBS
static inline void
DPR_SRB(char *where)
{
    if (srbs_seen != srbs_returned) {
        PRINTK(("%s: srbs_seen = %x, srbs_returned = %x, diff %d.\n",
            where, srbs_seen, srbs_returned,
            srbs_seen - srbs_returned));
    }
    if (sio_srbs_seen != sio_srbs_returned) {
        PRINTK(("%s: sio_srbs_seen = %x, sio_srbs_returned = %x, diff %d.\n",
            where, sio_srbs_seen, sio_srbs_returned,
            sio_srbs_seen - sio_srbs_returned));
    }
    if (io_srbs_seen != io_srbs_returned) {
        PRINTK(("%s: io_srbs_seen = %x, io_srbs_returned = %x, diff %d.\n",
            where, io_srbs_seen, io_srbs_returned,
            io_srbs_seen - io_srbs_returned));
    }
}
#else
#define DPR_SRB(_w)
#endif
#else
#define VBIF_INC_SRB(_V)
#define VBIF_DEC_SRB(_V)
#define DPR_SRB(_where)
#endif

/*
 * Miniport entry point decls.
 */

ULONG KvmDriverEntry(IN PVOID DriverObject, IN PVOID RegistryPath);

NTSTATUS sp_find_adapter(
    IN PVOID dev_ext,
    IN PVOID HwContext,
    IN PVOID BusInformation,
    IN PCSTR ArgumentString,
    IN OUT PPORT_CONFIGURATION_INFORMATION ConfigInfo,
    OUT PBOOLEAN Again);

BOOLEAN sp_initialize(virtio_sp_dev_ext_t *dev_ext);
BOOLEAN sp_passive_init(virtio_sp_dev_ext_t *dev_ext);
BOOLEAN sp_start_io(virtio_sp_dev_ext_t *dev_ext,
    PSCSI_REQUEST_BLOCK Srb);
BOOLEAN sp_build_io(virtio_sp_dev_ext_t *dev_ext,
    PSCSI_REQUEST_BLOCK Srb);
BOOLEAN sp_reset_bus(virtio_sp_dev_ext_t *dev_ext, ULONG PathId);
BOOLEAN sp_interrupt(IN PVOID dev_ext);
SCSI_ADAPTER_CONTROL_STATUS sp_adapter_control(
    IN PVOID dev_ext,
    IN SCSI_ADAPTER_CONTROL_TYPE control_type,
    IN PVOID parameters);

#ifdef CAN_USE_MSI
BOOLEAN sp_msinterrupt_routine(virtio_sp_dev_ext_t *dev_ext, ULONG  msg_id);
#endif

/* Calls into the specific drivers */
void virtio_sp_get_device_config(virtio_sp_dev_ext_t *dev_ext);
void virtio_sp_dump_device_config_info(virtio_sp_dev_ext_t *dev_ext,
    PPORT_CONFIGURATION_INFORMATION config_info);
void virtio_sp_initialize(virtio_sp_dev_ext_t *dev_ext);
NTSTATUS virtio_sp_find_vq(virtio_sp_dev_ext_t *dev_ext);
BOOLEAN virtio_sp_complete_cmd(virtio_sp_dev_ext_t *dev_ext,
                            ULONG reason,
                            ULONG  msg_id);
#ifdef USE_STORPORT_DPC
void virtio_sp_int_dpc(PSTOR_DPC Dpc, PVOID context, PVOID s1, PVOID s2);
#endif

BOOLEAN virtio_scsi_do_cmd(virtio_sp_dev_ext_t *dev_ext,
    SCSI_REQUEST_BLOCK *srb);

#ifdef IS_STORPORT
BOOLEAN virtio_sp_scsi_do_cmd(virtio_sp_dev_ext_t *dev_ext,
    SCSI_REQUEST_BLOCK *srb);
#define virtio_sp_do_cmd virtio_sp_scsi_do_cmd
#else
#define virtio_sp_do_cmd virtio_scsi_do_cmd
#endif

BOOLEAN virtio_sp_do_poll(virtio_sp_dev_ext_t *dev_ext, void *not_used);
void virtio_sp_poll(IN virtio_sp_dev_ext_t *dev_ext);

#ifdef DBG
extern ULONG g_int_to_send;

void virtio_sp_verify_sgl(virtio_sp_dev_ext_t *dev_ext,
    PSCSI_REQUEST_BLOCK srb,
    STOR_SCATTER_GATHER_LIST *sgl);
#else
#define virtio_sp_verify_sgl(_dev_ext, _srb, _sgl)
#endif

#endif  /* _VIRTIO_SP_COMMON_H */
