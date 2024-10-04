import hashlib
import sqlite3

def hash_password(password):
    return hashlib.sha1(password.encode()).digest()


def check_password_in_db(db_path, password_hash):
    with sqlite3.connect(db_path) as conn:
        cursor = conn.cursor()
        query = "SELECT count FROM pwned_passwords WHERE full_hash = ?"
        cursor.execute(query, (password_hash,))
        result = cursor.fetchone()
        return result[0] if result else 0

def get_valid_response(prompt="\nCheck another? (y/n): "):

    while True:
        response = input(prompt).strip().lower()
        
        if response in ['y', 'n']:
            return response.upper()
        else:
            print("Invalid input, please enter 'y' or 'n'.")