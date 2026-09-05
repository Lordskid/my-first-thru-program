# My First Thru Program

A beginner-friendly stateful program written in C for the Thru blockchain.

## What This Demonstrates

- Creating a program-owned account
- Persistent on-chain state
- Incrementing a counter
- Reading stored state
- Emitting events
- Using a PDA and state proof

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

```bash
make clean && make
```

Binary:

```text
build/thruvm/bin/my_first_thru_program_c.bin
```

## Instructions

| Instruction | Value | Purpose |
|---|---:|---|
| Create | 0 | Creates the counter account and initializes it to 0 |
| Increment | 1 | Increases the counter by 1 |
| Read | 2 | Reads the counter and emits its value |

## Counter Account

The counter account is derived from the program using the seed `count_acc`.

```bash
thru program derive-address <PROGRAM_ADDRESS> count_acc
```

## Create

Generate a fresh state proof:

```bash
PROOF_HEX="$(thru --json txn make-state-proof creating <COUNTER_PDA> | python3 -c 'import sys,json; print(json.load(sys.stdin)["makeStateProof"]["proof_data_hex"])' )"
```

Build the instruction:

```bash
SEED_HEX="636f756e745f6163630000000000000000000000000000000000000000000000"
INSTRUCTION_HEX="00000000""0200""$SEED_HEX""68000000""$PROOF_HEX"
```

Execute:

```bash
thru --json txn execute --fee 0 --readwrite-accounts <COUNTER_PDA> <PROGRAM_ADDRESS> "$INSTRUCTION_HEX"
```

## Increment

Instruction:

```text
010000000200
```

Execute it:

```bash
thru --json txn execute --readwrite-accounts <COUNTER_PDA> <PROGRAM_ADDRESS> 010000000200
```

Each successful execution increases the counter by 1 and emits the new value as an event.

## Read

Instruction:

```text
020000000200
```

Execute it:

```bash
thru --json txn execute --readwrite-accounts <COUNTER_PDA> <PROGRAM_ADDRESS> 020000000200
```

The program reads the stored value and emits it without changing the counter.

## Verify State

```bash
thru --json account info <COUNTER_PDA>
```

The counter is stored as an 8-byte unsigned integer.

## Upgrade

After modifying the program:

```bash
make clean && make
thru --json program upgrade <PROGRAM_SEED> build/thruvm/bin/my_first_thru_program_c.bin
```

## Project Structure

```text
my-first-thru-program/
├── GNUmakefile
├── README.md
├── .gitignore
└── examples/
    ├── Local.mk
    └── my_first_thru_program.c
```
