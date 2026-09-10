# My First Thru Program

A beginner-friendly stateful program written in C for the Thru blockchain.

This project demonstrates how to build, deploy, interact with, and upgrade a stateful program on Thru.

## What This Demonstrates

- Building a C program for the Thru VM
- Creating a program-owned account
- Persistent on-chain state
- Program-derived account addresses (PDA)
- State proofs
- Incrementing stored state
- Reading stored state
- Resetting stored state
- Emitting events
- Upgrading a deployed program

## What We Built

The program implements four instructions:

| Instruction | Value | Purpose |
|---|---:|---|
| Create | `0` | Creates and initializes the counter account |
| Increment | `1` | Increases the counter by 1 |
| Read | `2` | Reads the stored counter value |
| Reset | `3` | Sets the counter back to `0` |

The counter is stored as an 8-byte unsigned integer in a program-owned account.

## On-Chain Deployment

**Network:** Thru Alphanet

**Program seed:**
```text
lordskid-program-1787514397
```

**Program account:**
```text
tam-e003Byvz-Vw9zTw-pUzDiJdN0Na5_rHw5fwTKvIKnI
```

**Counter account (PDA):**
```text
ta8F1fG1Xdh_mXICGXGFi4G-lwyJSKHf3TRoxR_G7qFZd6
```

**Counter seed:**
```text
count_acc
```

## Verified On-Chain

The counter was successfully created and initialized:

```text
0
```

Two increment transactions then changed the state:

```text
0 → 1 → 2
```

After upgrading the deployed program, the stored state was read again and returned:

```text
2
```

The READ transaction completed successfully with:

```text
execution_result: 0
vm_error: 0
user_error_code: 0
state_units_consumed: 0
```

The emitted event contained:

```text
0200000000000000
```

which represents the stored counter value `2`.

This confirmed that the program upgrade preserved the existing on-chain state.

The RESET instruction was then executed successfully, changing the counter from `2` back to `0`. A final READ confirmed the stored value was `0`.

The complete verified flow was:

```text
CREATE → 0 → INCREMENT → 1 → INCREMENT → 2
        → UPGRADE → 2 → RESET → 0 → READ → 0
```

The RESET instruction emits the new counter value as an event.

## Prerequisites

Install the Thru toolchain and C SDK:

```bash
thru dev toolchain install
thru dev sdk install c
```

Check the CLI:

```bash
thru --version
```

## Build

Build the program:

```bash
make clean && make
```

The compiled binary is:

```text
build/thruvm/bin/my_first_thru_program_c.bin
```

## Counter Account

The counter account is derived from the program using the seed `count_acc`:

```bash
thru program derive-address <PROGRAM_ADDRESS> count_acc
```

## Create the Counter

Generate a fresh state proof:

```bash
PROOF_HEX="$(thru --json txn make-state-proof creating <COUNTER_PDA> | python3 -c 'import sys,json; print(json.load(sys.stdin)["makeStateProof"]["proof_data_hex"])')"
```

Build the instruction:

```bash
SEED_HEX="636f756e745f6163630000000000000000000000000000000000000000000000"
INSTRUCTION_HEX="00000000""0200""$SEED_HEX""68000000""$PROOF_HEX"
```

Execute the CREATE instruction:

```bash
thru --json txn execute --fee 0 --readwrite-accounts <COUNTER_PDA> <PROGRAM_ADDRESS> "$INSTRUCTION_HEX"
```

## Increment

Instruction:

```text
010000000200
```

Execute:

```bash
thru --json txn execute --readwrite-accounts <COUNTER_PDA> <PROGRAM_ADDRESS> 010000000200
```

Each successful execution increases the counter by `1` and emits the new value as an event.

## Read

Instruction:

```text
020000000200
```

Execute:

```bash
thru --json txn execute --readwrite-accounts <COUNTER_PDA> <PROGRAM_ADDRESS> 020000000200
```

The READ instruction emits the stored counter value without changing the state.

## Verify State

```bash
thru --json account info <COUNTER_PDA>
```

The counter is stored as an 8-byte unsigned integer.

## Reset

Instruction:

```text
030000000200
```

Execute:

```bash
thru --json txn execute --readwrite-accounts <COUNTER_PDA> <PROGRAM_ADDRESS> 030000000200
```

The RESET instruction sets the stored counter value back to `0`.

## Read After Reset

Run the READ instruction again:

```bash
thru --json txn execute --readwrite-accounts <COUNTER_PDA> <PROGRAM_ADDRESS> 020000000200
```

A successful event containing `0000000000000000` confirms that the counter is `0`.

## Upgrade

After modifying the program:

```bash
make clean && make
```

Upgrade the deployed program:

```bash
thru --json program upgrade \
  <PROGRAM_SEED> \
  build/thruvm/bin/my_first_thru_program_c.bin
```
