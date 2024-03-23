#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>

std::string hashPassword(const std::string& password) {
    return std::to_string(password.length() * 143); // Simple and not secure
}

std::string caesarCipher(std::string text, int shift, bool encrypt) {
    const std::string letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()";
    int direction = encrypt ? 1 : -1;
    for (char& c : text) {
        size_t pos = letters.find(c);
        if (pos != std::string::npos) {
            int newPos = (pos + direction * shift + letters.length()) % letters.length();
            c = letters[newPos];
        }
    }
    return text;
}

std::string generatePassword(int length) {
    const std::string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()";
    std::string password;
    for (int i = 0; i < length; ++i) {
        password += characters[rand() % characters.length()];
    }
    return password;
}

void savePassword(const std::string& username, const std::string& website, const std::string& password) {
    std::ofstream userFile(username + "_data.txt", std::ios::app);
    if (userFile.is_open()) {
        std::string encrypted = caesarCipher(password, 5, true);
        userFile << website << " " << encrypted << "\n";
        userFile.close();
    }
    else {
        std::cerr << "Unable to open file for writing.\n";
    }
}

void signUp() {
    std::string username, password;
    std::cout << "Enter username: ";
    std::cin >> username;
    std::cout << "Enter password: ";
    std::cin >> password;

    std::ofstream userFile(username + "_data.txt");
    if (userFile.is_open()) {
        userFile << "Username: " << username << "\nPassword Hash: " << hashPassword(password) << "\n";
        userFile.close();
        std::cout << "User registered successfully!\n";
    }
    else {
        std::cout << "Unable to create user file. Registration failed.\n";
    }
}

bool authenticateUser(const std::string& username, const std::string& password) {
    std::ifstream userFile(username + "_data.txt");
    if (userFile.is_open()) {
        std::string line, storedHash;
        // Skip the username line
        std::getline(userFile, line);
        // Read the stored hash
        if (std::getline(userFile, line)) {
            storedHash = line.substr(line.find(": ") + 2);
            if (storedHash == hashPassword(password)) {
                return true;
            }
        }
    }
    return false;
}

void listSavedPasswords(const std::string& username) {
    std::ifstream userFile(username + "_data.txt");
    if (userFile.is_open()) {
        std::string line;
        // Skip the first two lines (username and password hash)
        std::getline(userFile, line);
        std::getline(userFile, line);
        std::cout << "Saved passwords for " << username << ":\n";
        while (std::getline(userFile, line)) {
            std::size_t pos = line.find(" ");
            if (pos != std::string::npos) {
                std::string website = line.substr(0, pos);
                std::string encryptedPassword = line.substr(pos + 1);
                std::cout << "Website: " << website << ", Password: " << caesarCipher(encryptedPassword, 5, false) << std::endl;
            }
        }
        userFile.close();
    }
    else {
        std::cerr << "Unable to open file for reading.\n";
    }
}

void displayUserMenu() {
    std::cout << "Welcome to your Menu make your choice:\n";
    std::cout << "1. > Generate and save a new password\n";
    std::cout << "2. > List all saved passwords\n";
    std::cout << "3. > Logout\n";
    std::cout << "4. > Delete Account\n";
    std::cout << "5. > Update Password\n"; // Updated text for clarity
    std::cout << "> Enter your choice: ";
}

void deleteAccount(const std::string& username) {
    std::string filename = username + "_data.txt";
    if (remove(filename.c_str()) != 0) {
        std::cerr << "Error deleting account.\n";
    }
    else {
        std::cout << "Account deleted successfully.\n";
    }
}

void updatePassword(const std::string& username) {
    std::string newPassword;
    std::cout << "Enter your new password: ";
    std::cin >> newPassword;

    // Assuming you want to overwrite the existing file with new password
    std::ofstream userFile(username + "_data.txt");
    if (userFile.is_open()) {
        userFile << "Username: " << username << "\nPassword Hash: " << hashPassword(newPassword) << "\n";
        std::cout << "Password updated successfully.\n";
    }
    else {
        std::cerr << "Error updating password.\n";
    }
    userFile.close();
}

void login() {
    std::string username, password;
    std::cout << "Enter username: ";
    std::cin >> username;
    std::cout << "Enter password: ";
    std::cin >> password;

    if (authenticateUser(username, password)) {
        std::cout << "Logged in successfully!\n";
        int userChoice;
        do {
            displayUserMenu();
            std::cin >> userChoice;
            switch (userChoice) {
            case 1: {
                std::string website;
                std::cout << "Enter the website for which you want to generate a password: ";
                std::cin >> website;

                // Optional: Let user choose the password length or set a default value
                int length = 12; // Default length
                std::cout << "Enter password length (Recommended: 12): ";
                std::cin >> length;

                std::string newPassword = generatePassword(length);
                std::cout << "Generated Password: " << newPassword << std::endl;

                savePassword(username, website, newPassword);
                std::cout << "Password saved successfully!" << std::endl;
                break;
            }
            case 2:
                listSavedPasswords(username);
                break;
            case 3:
                std::cout << "Logging out...\n";
                break;
            case 4:
                deleteAccount(username);
                return; // Exit after deleting account
            case 5:
                updatePassword(username);
                std::cout << "Please log in again with your new password.\n";
                return; // Suggesting re-login after password change for validation
            default:
                std::cout << "Invalid choice, please try again.\n";
            }
        } while (userChoice != 3 && userChoice != 4 && userChoice != 5);
    }
    else {
        std::cout << "Invalid username or password.\n";
    }
}

void displayPasswordManagerMenu() {
    std::cout << "Welcome to LockWarden Password Manager Menu:\n";
    std::cout << "1. > Sign Up\n";
    std::cout << "2. > Log In\n";
    std::cout << "3. > Exit\n";
    std::cout << "Enter your choice: ";
}

int main() {
    srand(time(NULL)); // Initialize random seed

    while (true) {
        displayPasswordManagerMenu();
        int choice;
        std::cin >> choice;

        switch (choice) {
        case 1:
            signUp();
            break;
        case 2:
            login();
            break;
        case 3:
            std::cout << "Exiting Password Manager.\n";
            return 0;
        default:
            std::cout << "Invalid choice, please try again.\n";
        }
    }

    return 0;
}
