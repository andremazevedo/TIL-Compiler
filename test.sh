#!/bin/zsh

# Directories
TESTS_DIR=auto-tests
EXPECTED_DIR=$TESTS_DIR/expected

# Color codes
PASS='\033[0;32mPASSED\033[0m'
FAIL='\033[0;31mFAILED\033[0m'

# Log file
LOGFILE=log.md

# Tex Color to md
LOG_PASS='$\\color{green}{\\textbf{PASSED}}$'
LOG_FAIL='$\\color{red}{\\textbf{FAILED}}$'

# Determine if a specific test file was provided as an argument
if [ $# -eq 1 ]; then
  TEST_FILES=($1)
  ALL_TESTS=false
else
  TEST_FILES=($TESTS_DIR/*.til)
  ALL_TESTS=true
fi

# Clear previous log
if $ALL_TESTS
then
  rm -f $LOGFILE
fi

# Compile the project
echo "Compiling the project..."
make > /dev/null
if [ $? -ne 0 ]; then
  echo "Compilation failed"
  if $ALL_TESTS
  then
    echo "Compilation failed" > $LOGFILE
  fi
  exit
fi

# Function to clean up generated files
cleanup_files() {
  rm -f $asm_file $obj_file $exec_file $out_file
}

# Iterate through each .til file in auto-tests directory
for test_file in $TEST_FILES
do
  test_name=$(basename $test_file .til)
  asm_file=$TESTS_DIR/$test_name.asm
  obj_file=$TESTS_DIR/$test_name.o
  exec_file=$TESTS_DIR/$test_name
  out_file=$TESTS_DIR/$test_name.out

  echo "Testing $test_name..."

  # Generate assembly code
  if $ALL_TESTS
  then
    ./til -g --target asm $test_file > /dev/null 2>&1
  else
    ./til -g --target asm $test_file
  fi

  if [ $? -ne 0 ]; then
    echo -e "Test $test_name: $FAIL: Failed to generate assembly"
    if $ALL_TESTS
    then
      echo "Test $test_name: $LOG_FAIL: Failed to generate assembly" >> $LOGFILE
      cleanup_files
    fi
    continue
  fi

  # Assemble the .asm file
  if $ALL_TESTS
  then
    yasm -felf32 -o $obj_file $asm_file > /dev/null 2>&1
  else
    yasm -felf32 -o $obj_file $asm_file
  fi

  if [ $? -ne 0 ]; then
    echo -e "Test $test_name: $FAIL: Assembly failed"
    if $ALL_TESTS
    then
      echo "Test $test_name: $LOG_FAIL: Assembly failed" >> $LOGFILE
      cleanup_files
    fi
    continue
  fi

  # Link the object file
  if $ALL_TESTS
  then
    ld -melf_i386 -o $exec_file $obj_file -lrts > /dev/null 2>&1
  else
    ld -melf_i386 -o $exec_file $obj_file -lrts
  fi

  if [ $? -ne 0 ]; then
    echo -e "Test $test_name: $FAIL: Linking failed"
    if $ALL_TESTS
    then
      echo "Test $test_name: $LOG_FAIL: Linking failed  " >> $LOGFILE
      cleanup_files
    fi
    continue
  fi

  # Run the executable and capture the output
  ./$exec_file > $out_file
  exec_status=$?
  if [ $exec_status -ne 0 ] && [ $exec_status -ne 1 ]; then
    echo -e "Test $test_name: $FAIL: Execution failed"
    if $ALL_TESTS
    then
      echo "Test $test_name: $LOG_FAIL: Execution failed  " >> $LOGFILE
      cleanup_files
    fi
    continue
  fi

  # Compare the output with the expected output
  diff -iwub =(tr -d '[:space:]' < $out_file) =(tr -d '[:space:]' < $EXPECTED_DIR/$test_name.out) > /dev/null
  if [ $? -eq 0 ]; then
    echo -e "Test $test_name: $PASS"
    if $ALL_TESTS
    then
      echo "Test $test_name: $LOG_PASS  " >> $LOGFILE
    fi
  else
    echo -e "Test $test_name: $FAIL: Output mismatch"
    if $ALL_TESTS
    then
      echo "Test $test_name: $LOG_FAIL: Output mismatch$  " >> $LOGFILE
    fi
  fi

  # Clean up generated files
  if $ALL_TESTS
  then
    cleanup_files
  fi
done

echo "All tests completed."
