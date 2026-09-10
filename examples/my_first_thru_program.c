#include <thru-sdk/c/tn_sdk.h>
#include <thru-sdk/c/tn_sdk_syscall.h>

/* Instruction types */
#define INSTRUCTION_CREATE    0U
#define INSTRUCTION_INCREMENT 1U
#define INSTRUCTION_READ      2U
#define INSTRUCTION_RESET     3U

/* Persistent state stored in the program-owned counter account. */
typedef struct __attribute__((packed)) {
    ulong counter_value;
} counter_account_t;

/* Arguments used when creating the counter account. */
typedef struct __attribute__((packed)) {
    uint instruction_type;
    ushort account_index;
    uchar seed[TN_SEED_SIZE];
    uint proof_size;
} create_args_t;

/* Arguments used by increment and read instructions. */
typedef struct __attribute__((packed)) {
    uint instruction_type;
    ushort account_index;
} counter_args_t;

/*
 * Program entrypoint.
 *
 * The instruction type determines which operation is executed:
 *   0 = create counter account
 *   1 = increment counter
 *   2 = read counter
 *   3 = reset counter
 */
TSDK_ENTRYPOINT_FN void
start(void) {

    tsdk_txn_t const *txn = tsdk_get_txn();

    uchar const *data = tsdk_txn_get_instr_data(txn);
    ulong data_size = tsdk_txn_get_instr_data_sz(txn);

    if (data_size < sizeof(uint)) {
        tsdk_revert(1UL);
    }

    uint instruction_type =
        *(uint const *)data;

    if (instruction_type == INSTRUCTION_CREATE) {

        if (data_size < sizeof(create_args_t)) {
            tsdk_revert(2UL);
        }

        create_args_t const *args =
            (create_args_t const *)data;

        ulong expected_size =
            sizeof(create_args_t) + args->proof_size;

        if (data_size != expected_size) {
            tsdk_revert(3UL);
        }

        uchar const *proof =
            data + sizeof(create_args_t);

        ulong result = tsys_account_create(
            args->account_index,
            args->seed,
            proof,
            args->proof_size
        );

        if (result != TSDK_SUCCESS) {
            tsdk_revert(result);
        }

        result = tsys_set_account_data_writable(
            args->account_index
        );

        if (result != TSDK_SUCCESS) {
            tsdk_revert(5UL);
        }

        result = tsys_account_resize(
            args->account_index,
            sizeof(counter_account_t)
        );

        if (result != TSDK_SUCCESS) {
            tsdk_revert(6UL);
        }

        counter_account_t *counter =
            (counter_account_t *)
            tsdk_get_account_data_ptr(args->account_index);

        if (counter == 0) {
            tsdk_revert(7UL);
        }

        counter->counter_value = 0UL;

        tsdk_return(TSDK_SUCCESS);
    }

    if (instruction_type == INSTRUCTION_INCREMENT) {

        if (data_size != sizeof(counter_args_t)) {
            tsdk_revert(8UL);
        }

        counter_args_t const *args =
            (counter_args_t const *)data;

        if (!tsdk_is_account_idx_valid(args->account_index)) {
            tsdk_revert(9UL);
        }

        if (!tsdk_is_account_owned_by_current_program(
                args->account_index)) {
            tsdk_revert(10UL);
        }

        ulong result =
            tsys_set_account_data_writable(args->account_index);

        if (result != TSDK_SUCCESS) {
            tsdk_revert(11UL);
        }

        counter_account_t *counter =
            (counter_account_t *)
            tsdk_get_account_data_ptr(args->account_index);

        if (counter == 0) {
            tsdk_revert(12UL);
        }

        counter->counter_value++;

        tsys_emit_event(
            (uchar const *)&counter->counter_value,
            sizeof(ulong)
        );

        tsdk_return(TSDK_SUCCESS);
    }

    if (instruction_type == INSTRUCTION_READ) {

        if (data_size != sizeof(counter_args_t)) {
            tsdk_revert(14UL);
        }

        counter_args_t const *args =
            (counter_args_t const *)data;

        if (!tsdk_is_account_idx_valid(args->account_index)) {
            tsdk_revert(15UL);
        }

        if (!tsdk_is_account_owned_by_current_program(
                args->account_index)) {
            tsdk_revert(16UL);
        }

        counter_account_t const *counter =
            (counter_account_t const *)
            tsdk_get_account_data_ptr(args->account_index);

        if (counter == 0) {
            tsdk_revert(17UL);
        }

        tsys_emit_event(
            (uchar const *)&counter->counter_value,
            sizeof(ulong)
        );

        tsdk_return(TSDK_SUCCESS);
    }

    if (instruction_type == INSTRUCTION_RESET) {

        if (data_size != sizeof(counter_args_t)) {
            tsdk_revert(18UL);
        }

        counter_args_t const *args =
            (counter_args_t const *)data;

        if (!tsdk_is_account_idx_valid(args->account_index)) {
            tsdk_revert(19UL);
        }

        if (!tsdk_is_account_owned_by_current_program(
                args->account_index)) {
            tsdk_revert(20UL);
        }

        ulong result =
            tsys_set_account_data_writable(args->account_index);

        if (result != TSDK_SUCCESS) {
            tsdk_revert(21UL);
        }

        counter_account_t *counter =
            (counter_account_t *)
            tsdk_get_account_data_ptr(args->account_index);

        if (counter == 0) {
            tsdk_revert(22UL);
        }

        counter->counter_value = 0UL;

        tsys_emit_event(
            (uchar const *)&counter->counter_value,
            sizeof(ulong)
        );

        tsdk_return(TSDK_SUCCESS);
    }

    tsdk_revert(13UL);
}
