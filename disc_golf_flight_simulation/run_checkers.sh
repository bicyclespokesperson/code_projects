#!/usr/bin/env bash

set -e

cargo clippy --fix
cargo fmt
