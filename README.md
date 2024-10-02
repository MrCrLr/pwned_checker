#Pwned Checker

A C-based command-line tool to check if a given password has been compromised using the SHA-1 hashes from the Have I Been Pwned database. This program leverages an SQLite database to store and search through the list of pwned password hashes.

#Features

• Secure password input: Passwords are masked during input, and memory is securely wiped after processing.

• Deep password check: The program uses SHA-1 hashes to query a pre-built SQLite database to check if the password has been compromised.

• Efficient query performance: Indexed queries ensure that even large databases of passwords can be searched quickly.

• Multiple password checking: You can check multiple passwords in a single session.


#Requirements

• GCC (or any other C compiler)

• SQLite3

• OpenSSL for SHA-1 hash calculation

• The pwnedpasswords.db file, created from the pwnedpasswords.txt file using create_database.


Installation

1. Clone the repository:

git clone https://github.com/yourusername/pwned_checker.git

cd pwned_checker

2. Build the project:

cd build

make

3. Download the pwned passwords data from the Have I Been Pwned service:

This will vary depending on the service; it could be done using a tool like the haveibeenpwned-downloader.

4. Create the SQLite database:

./bin/create_database database/pwnedpasswords.db resources/pwnedpasswords.txt

#Usage

Once the database is set up, you can run the checker program as follows:

./bin/pwned_checker

Example:

$ ./pwned_checker

Enter your password: *****

Password found in pwned list with 273646 occurrences!

Check complete.


Do you want to check another password? (y/n): n

Exiting program.


Running multiple password checks:

You can check multiple passwords in a single session. After checking a password, you will be prompted to enter another one or exit.

#File Structure

pwned_checker/
│
├── bin/                        # Compiled executables
├── build/                      # Build directory with object files
├── database/                   # Source and pre-built SQLite database file
│   ├── create_database.c       # Code to create the SQLite database
│   ├── pwnedpasswords.db       # SQLite database (generated from pwnedpasswords.txt)
├── include/                    # Header files for the project
├── resources/                  # Resources like the original password text file
│   └── pwnedpasswords.txt      # Raw text file containing SHA-1 hashes
├── src/                        # Source code
│   ├── deep_check.c            # Performs the SQLite deep check of the password
│   ├── main.c                  # Main program logic
│   ├── password_input.c        # Secure password input handling
│   ├── utils.c                 # Utility functions (secure memory handling, etc.)
├── lib/                        # External libraries (optional)
├── .gitignore                  # Files ignored by Git
└── README.md                   # This file


#Security Considerations

• Memory Scrubbing: The program ensures sensitive information (like passwords) is scrubbed from memory before being freed.

• Password Masking: The entered password is masked during input to prevent shoulder-surfing attacks.


#Contributing

If you’d like to contribute to this project, feel free to open a pull request or issue. Improvements, especially regarding performance or security, are always welcome.


License

This project is licensed under the MIT License. See the LICENSE file for details.
