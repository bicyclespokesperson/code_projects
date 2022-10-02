#!/usr/bin/env bash

set -e

# Disable --fix so I can see the errors
cargo clippy
cargo fmt
