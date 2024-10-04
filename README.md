# Pwned Checker

Pwned Checker is a C-based command-line application designed to help individuals determine if their passwords have been compromised and exposed in known data breaches. This tool uses the SHA-1 hash algorithm to compare entered passwords against a comprehensive database of compromised credentials provided by the Have I Been Pwned (HIBP) service. The core functionality of Pwned Checker utilizes an SQLite database, which stores over 930 million pwned password hashes with the number of times each has appeared. This project is crafted to offer both security and efficiency, ensuring that user inputs are handled securely while maintaining high-performance database queries.

The Pwned Checker project provides a command-line interface where users can enter their passwords. It includes several C source files, each serving a distinct role from managing database connections, handling user input securely, to performing the checks necessary to assess password security. This README aims to guide you through the project’s functionality, setup, and use, explaining the purpose behind each component and the design choices made during development.

## Secure Password Input

Pwned Checker prioritizes security in its operations. The program masks all password inputs during the entry process to ensure that passwords are never displayed on the screen, reducing the risk of over-the-shoulder attacks. Furthermore, once a password is processed, its memory representation is securely wiped to prevent any chance of memory-based attacks. This feature leverages custom functions that overwrite and then free the memory used to store password data.

## Deep Password Check

Utilizing the SHA-1 hashing algorithm, Pwned Checker transforms the user’s password into a hash and queries an SQLite database to check if this hash has previously been exposed in a data breach. This database query is optimized through the use of indexes, ensuring that even the large datasets (nearly one billion(!) records) are quickly searchable. The hash comparison is done securely on the user’s local machine to ensure that the actual password never leaves the system, thereby maintaining user privacy and security.

## Multiple Password Checking

Pwned Checker supports checking multiple passwords in a single session. After the user's entry, the application reponds with whether or not the password has appeared in a data breach and, if so, the number of times. The program then promptly asks the user if they would like to check another.


## Requirements

• GCC (or any other C compiler)

• SQLite3

• OpenSSL for SHA-1 hash calculation

• The pwnedpasswords.db file, created from the pwnedpasswords.txt file using create_database.


## Installation

1. Clone the repository:

git clone https://github.com/heavenlyblues/pwned_checker.git
cd pwned_checker

2. Build the project:

cd build
make

3. Download the pwned passwords data from the Have I Been Pwned service:

This will vary depending on the service; it could be done using a tool like the haveibeenpwned-downloader.

4. Create the SQLite database:

./bin/create_database database/pwnedpasswords.db resources/pwnedpasswords.txt

## Usage

Once the database is set up, you can run the checker program as follows:
./bin/pwned_checker

Example:

$ ./pwned_checker
Enter your password: *****
Password found in pwned list with 273646 occurrences!
Check complete.

Do you want to check another password? (y/n): n
Exiting program.



## File Structure

pwned_checker/

bin/ # Compiled executables

build/ # Build directory with object files

database/ # Source and pre-built SQLite database file

create_database_main.c # Main to run database creation

create_database.c # Code to create the SQLite database

pwnedpasswords.db # SQLite database (generated from pwnedpasswords.txt)

include/ # Header files for the project

resources/ # Resources like the original password text file

pwnedpasswords.txt # Raw text file containing SHA-1 hashes

src/ # Source code

deep_check.c # Performs the SQLite deep check of the password

main.c # Main program logic

password_input.c # Secure password input and memory handling

utils.c # Escape sequence handling and input utilities

README.md # This file




## Contributing

If you’d like to contribute to this project, feel free to open a pull request or issue. Improvements, especially regarding performance or security, are always welcome.


License

This project is licensed under the MIT License. See the LICENSE file for details.
