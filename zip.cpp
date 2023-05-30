#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <zip.h>
// includes test
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> // Required for functions in Linux

using namespace std;
namespace fs = filesystem;

void writeStringToFile(const string &str, const string &fileName, mode_t permissions)
{
    fs::create_directories(fs::path(fileName).parent_path()); // create directories if isn't exist
    ofstream file(fileName, ios::trunc);
    if (file.is_open())
    {
        file << str;
        // Set the premissions using chmod function
        chmod(fileName.c_str(), permissions);
        file.close();
        cout << "String written to file " << fileName << " successfully." << endl;
    }
    else
    {
        cerr << "Failed to open the file" << fileName << "." << endl;
    }
}

void zipFlightsDB()
{
    const string flightsDBFolderPath = "./flightsDB";
    const string zipFileName = "./flightsDB.zip";
    // Check if FlightsDB folder exists
    if (!fs::is_directory(flightsDBFolderPath))
    {
        cerr << "FlightsDB folder not found." << endl;
        return;
    }

    // Create a new zip archive
    zip_t *archive = zip_open(zipFileName.c_str(), ZIP_CREATE | ZIP_TRUNCATE, NULL);
    if (!archive)
    {
        cerr << "Failed to create the zip archive." << endl;
        return;
    }

    // Iterate over airport folders in FlightsDB
    for (const auto &entry : fs::directory_iterator(flightsDBFolderPath))
    {
        if (entry.is_directory())
        {
            string airportCode = entry.path().filename().string();
            string airportFolderPath = "flightsDB/" + airportCode;
            for (const auto &airportEntry : fs::directory_iterator(airportFolderPath))
            {
                string fileName = airportEntry.path().filename().string();
                string filePath = airportFolderPath + "/" + fileName;

                zip_source_t *source = zip_source_file(archive, filePath.c_str(), 0, -1); // create zip_source from file text
                if (!source)                                                              // Error if can't create zip source
                {
                    cerr << "Error: create zip source for file " << fileName << endl;
                    continue;
                }
                zip_int64_t error = zip_file_add(archive, filePath.c_str(), source, ZIP_FL_ENC_UTF_8); // adding the source to the zip file
                if (error == -1)                                                                       // Error if can't add from zip source
                {
                    cerr << "Error: add file " << fileName << " to zip archive " << endl;
                    zip_source_free(source);
                    continue;
                }
            }
        }
    }
    if (zip_close(archive) == -1)
        cerr << "flightsDB has not been zipped !" << endl;
    else
        cout << "lightsDB has been zipped successfully!" << endl;
}

void unzipFlightsDB(mode_t permissions)
{
    const string flightsDBFolderPath = "./flightsDB";
    const string zipFileName = "./flightsDB.zip";

    // Check if the zip file exists
    if (!fs::exists(zipFileName))
    {
        cerr << "flightsDB.zip file not found." << endl;
        return;
    }

    // Open the zip archive
    zip_t *archive = zip_open(zipFileName.c_str(), 0, NULL);
    if (!archive)
    {
        cerr << "Failed to open the zip archive." << endl;
        return;
    }

    zip_int64_t num_of_zip_entries = zip_get_num_entries(archive, ZIP_FL_UNCHANGED);
    zip_stat_t file_stats;
    cout << "Number of entries: " << num_of_zip_entries << endl;
    for (int i = 0; i < num_of_zip_entries; i++)
    {
        int error = zip_stat_index(archive, i, 0, &file_stats);
        zip_file_t *file = zip_fopen_index(archive, i, ZIP_FL_UNCHANGED);
        if (file == nullptr || error == -1)
        {
            cerr << "Failed to open file from zip archive or get stats of file." << endl;
            continue;
        }
        const char *fileName = file_stats.name;
        cout << i << ": " << fileName << endl;
        string fileData;
        fileData.resize(file_stats.size + 1);
        zip_fread(file, fileData.data(), file_stats.size);
        writeStringToFile(fileData, "./" + string(file_stats.name), permissions);
    }
    if (zip_close(archive) == -1)
        cout << "flightsDB has been failed to unzipped in the process!" << endl;
    else
        cout << "FlightsDB has been unzipped successfully!" << endl;
}

int main(int argc, char *argv[])
{
    mode_t permissions = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; // permissions: read for all, write only for user.
    // zipFlightsDB();
    unzipFlightsDB(permissions);
    return 0;
}
