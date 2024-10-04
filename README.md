# PWNED CHECKER
#### Video Demo: https://youtu.be/3UGi0ikbMjU

#### Description: Pwned Checker is a C-based command-line application designed to help individuals determine if their passwords have been compromised and exposed in known data breaches. 

With the increasing number of data breaches affecting millions of users worldwide, many individuals are unaware that their passwords might already be compromised and exposed in online leaks. Pwned Checker addresses this issue by allowing users to verify the security of their passwords, checking them against a comprehensive database of compromised credentials.

The application uses the SHA-1 hashing algorithm to securely compare entered passwords against a massive database provided by the Have I Been Pwned (HIBP) service. HIBP is a well-known online service that tracks and collects data from breaches, amassing millions of exposed credentials to help users verify whether their information has been compromised. Pwned Checker integrates this dataset into a local SQLite database that stores over 930 million pwned password hashes. This ensures users can check their passwords locally, without the need to send them over the internet, thus maintaining privacy and security.

##Key Features of Pwned Checker

### 1. Local Database of Compromised Passwords

Pwned Checker relies on an SQLite database to store over 930 million password hashes, representing all known compromised passwords tracked by HIBP. Each password hash is paired with a count representing how many times that password has appeared in known data breaches. This setup allows for efficient querying and ensures that users can quickly check their passwords without waiting for online API responses.

### 2. SHA-1 Hashing for Secure Comparison

When a user enters their password, Pwned Checker transforms it into a 20 byte binary SHA-1 hash before querying the database. This ensures that the actual password is never directly exposed in memory or compared with plaintext passwords. Using SHA-1 for hashing ensures compatibility with the HIBP dataset, which I converted from hex digit strings when building the database to conserve storage space.

### 3. Efficient Database Queries

The SQLite database used by Pwned Checker is optimized for speed, using indexes on the hashed passwords (full_hash attribute as well as the count attribute) to ensure efficient querying even with such a large dataset. With over 930 million records, performance was a critical concern, and indexing ensured that password lookups are completed in a fraction of a second.

### 4. Multiple Password Checks per Session

Pwned Checker supports checking multiple passwords within a single session. After entering a password and receiving a response, the user is prompted to check another password if desired.


## Secure Password Input

Pwned Checker prioritizes security, especially regarding how passwords are entered and stored in memory. Password input is handled using custom functions that ensure the password is not echoed to the terminal during entry, preventing prying eyes from gaining anything from peeking over your shoulder. Instead of displaying the characters, the input is masked with asterisks, nicely providing the user with feedback that keystrokees are in fact being registered.

After the password is entered and hashed, its memory representation is immediately overwritten and securely freed. This prevents memory-based attacks or potential vulnerabilities that could exploit sensitive information left in memory after it’s no longer needed. By clearing the memory used for password storage, Pwned Checker ensures that sensitive data is handled properly throughout the program’s execution.


## Database Structure and Design Choices

Pwned Checker uses an SQLite database to store compromised password hashes. This was a critical design decision based on performance and security needs. Initially, searching through the plaintext HIBP dataset was too slow to be practical, given the massive number of entries. The solution was to build a local SQLite database with an indexed column for the SHA-1 password hashes. By leveraging SQLite’s indexing capabilities, Pwned Checker can search the database quickly, even with nearly a billion records.

The database stores the password hashes as 20-byte binary hashes instead of longer hexadecimal strings. This decision was made to reduce the database size and improve performance. The binary representation is more compact than hex strings, allowing the database to handle large amounts of data more efficiently. Additionally, storing passwords in binary form ensures that database lookups are faster, as binary comparisons are more efficient than comparing longer hex strings.


## Deep Password Check

At its core, Pwned Checker performs what is known as a deep password check. After the user enters their password, it is hashed using SHA-1 and then compared against the locally stored hash values. If a matching hash is found in the database, the program retrieves the count of how many times that password has appeared in breaches and informs the user. The check is performed locally on the user’s machine, so at no point does the actual password leave the system. This ensures maximum privacy and security, as even the hashed version of the password is never sent to an external service.


## Failed Bloom Filter Implementation

During the development process, an early approach involved attempting use of a Bloom filter to speed up password lookups. Despite initial hopes that the Bloom filter would help filter out non-matching passwords, it ultimately proved too slow for such a large dataset. The SQLite database solution was a much more efficient and reliable alternative.


## Conclusion

Pwned Checker provides a robust solution for users looking to check the security of their passwords in a secure and efficient manner. By implementing secure password input handling, flexible memory management, and leveraging a highly optimized SQLite database, the program is able to provide quick and reliable password checks for users concerned about data breaches. Although early attempts with a Bloom filter were unsuccessful, the final database approach ensures the program performs well even with a massive dataset of compromised credentials.


## Requirements

• GCC (or any other C compiler)

• SQLite3

• OpenSSL for SHA-1 hash calculation

• The pwnedpasswords.db file, created from the pwnedpasswords.txt file using create_database.


## Installation

### 1. Clone the repository:

git clone https://github.com/heavenlyblues/pwned_checker.git
cd pwned_checker

### 2. Build the project:

cd build
make

### 3. Download the pwned passwords data from the Have I Been Pwned service:

This will vary depending on the service; it could be done using a tool like the haveibeenpwned-downloader.

### 4. Create the SQLite database:

./bin/create_database database/pwnedpasswords.db resources/pwnedpasswords.txt

## Usage

Once the database is set up, you can run the checker program as follows:
./bin/pwned_checker

**Example:**

$ ./pwned_checker
Enter your password: *****
Password found in pwned list with 273646 occurrences!
Check complete.

Do you want to check another password? (y/n): n
Exiting program.



## File Structure

### pwned_checker/

**bin/** # Compiled executables

**build/** # Build directory with object files

**database/** # Source and pre-built SQLite database file

---create_database_main.c # Main to run database creation

---create_database.c # Code to create the SQLite database

---create_database.h

**include/** # Header files for the project

---deep_check.h

---password_input.h

---utils.h

**python_ver/** # Just for fun copy of the program in python

---main.py

---utils.py

**src/** # Source code

---deep_check.c # Performs the SQLite deep check of the password

---main.c # Main program logic

---password_input.c # Secure password input and memory handling

---utils.c # Escape sequence handling and input utilities

README.md # This file




### Contributing

If you’d like to contribute to this project, feel free to open a pull request or issue. Improvements, especially regarding performance or security, are always welcome.


### License

This project is licensed under the MIT License. See the LICENSE file for details.
