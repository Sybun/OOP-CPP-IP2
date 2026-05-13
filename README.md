# IntervalTree ADT

A fast, augmented Binary Search Tree for storing and querying overlapping intervals.

## Overview

**IntervalTree** efficiently finds which intervals overlap a given range in **O(log n)** time. It's useful for scheduling, memory management, and geometric queries.

The creation of this project was facilitated by Antropic AI tools.

## Key Features

- **Fast Overlap Search**: O(log n) instead of O(n)
- **CRUD Operations**: Insert, search, update, delete intervals
- **Deep Copy**: Independent copies via copy constructor/assignment
- **Exception Handling**: Validates interval bounds and detects duplicates
- **Operator Overloading**: Use `+=`, `-=`, `%=`, `[]`, `!` for intuitive API

## Building

```bash
make rebuild    # Clean build everything
make run_test   # Run all 40+ tests
make run_demo   # Run demo program
make clean      # Remove artifacts
