from utils import hash_password, check_password_in_db, get_valid_response
from getpass import getpass

def main():
    db_path = '../database/pwnedpasswords.db'
    
    try:
        while True:
            password = getpass(prompt="Enter your password: ")
            password_hash = hash_password(password)

            print(f"Password length: {len(password)} characters")
            print(f"Hashed password: {password_hash.hex()}")
            
            count = check_password_in_db(db_path, password_hash)
            
            if count > 0:
                print(f"Password has been breached {count} times.")
            else:
                print("Password is safe.")

            response = get_valid_response()

            if response == 'N':
                print("Exiting...\n")
                break
    
    except KeyboardInterrupt:
        print("Operation cancelled. Exiting gracefully...\n")

if __name__ == '__main__':
    main()