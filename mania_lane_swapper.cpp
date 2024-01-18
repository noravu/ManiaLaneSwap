#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>

//Console program for swapping lanes in an osu!mania beatmap v1.1
//Developed by volumni

int main()
{
    std::string input;
    std::cout << "osu!mania Lane Swapper" << std::endl;
    while (true) {
        input = "";
        std::cout << "Please choose a .osu beatmap file to edit. (this will be found in any subfolder of your 'AppData/Local/osu!/songs' folder). Type STOP to end the program." << std::endl;
        std::getline(std::cin, input);
        if (input == "STOP") {
            std::cout << "You can now safely close this program." << std::endl;
            exit(0);
        }
        if (input[0] == '"' && input[input.length() - 1] == '"') {
            input = input.substr(1, input.length() - 2);
        }
        if (input.length() < 5 || input.compare(input.length() - 4, 4, ".osu") != 0) {
            std::cout << "User input is not a valid .osu file. Be sure to type the whole file path." << std::endl;
            exit(0);
        }

        std::string lanes;
        const std::string defaultLanes = "1234";
        std::cout << "Please write new lane indices desired. (ex. 1234 is default, 4321 is equivalent to mirror mod)" << std::endl;
        std::cin >> lanes;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (!std::is_permutation(lanes.begin(), lanes.end(), defaultLanes.begin())) {
            std::cout << "User input is not a valid lane configuration. Format should match 1234" << std::endl;
            exit(0);
        }
        const std::string positions[4] = { "64", "192", "320", "448" };

        std::ifstream inputFile(input);
        if (!inputFile.is_open()) {
            std::cerr << "Unable to open the file." << std::endl;
            exit(1);
        }

        size_t position = 0;
        bool found = false;
        std::string line;
        std::string fileContents = "";
        while (std::getline(inputFile, line)) {
            fileContents += line;
            if (line.find("Version:") != std::string::npos) {
                fileContents += " (" + lanes + ")";
            }
            fileContents += "\n";
            if (line == "[HitObjects]") {
                found = true;
                position = inputFile.tellg();
                break;
            }
        }
        if (!found) {
            std::cout << "Hit objects not found." << std::endl;
            exit(1);
        }

        std::cout << "Swapping lanes in progress..." << std::endl;

        std::string token;
        inputFile.seekg(position);
        int index;
        bool x;
        while (std::getline(inputFile, line)) {
            std::istringstream iss(line);
            x = true;
            while (std::getline(iss, token, ',')) {
                if (x) {
                    index = std::distance(positions, std::find(positions, positions + 3, token) + 1);
                    fileContents += positions[lanes.find(std::to_string(index))];
                    x = false;
                }
                else {
                    fileContents += ',' + token;
                }
            }
            fileContents += '\n';
        }
        inputFile.close();

        input.erase(input.length() - 4, 4);
        input += " (" + lanes + ").osu";
        std::ofstream outputFile(input);
        if (!outputFile.is_open()) {
            std::cerr << "Unable to write to new file." << std::endl;
            exit(1);
        }

        outputFile << fileContents;
        outputFile.close();

        std::cout << "Swapping lanes complete!" << std::endl;
    }
    return 0;
}