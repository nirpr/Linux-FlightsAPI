#include "flightDB.h"
using namespace std;
namespace fs = std::filesystem;

// Methods
FlightDatabase::FlightDatabase(bool loadfromZip) noexcept(false) : loaded(false)
{
    if (loadfromZip)
    {
        try
        {
            FlightDatabase::unzipDB();
            load_DB_from_folder();
            loaded = true;
        }
        catch (const fs::filesystem_error &e)
        {
            if (e.code().value() != (int)errc::no_such_file_or_directory) // ignore if zip file not exist
                throw e;
        }
    }
}

void FlightDatabase::load_DB_from_folder(bool reRun = false) noexcept(false)
{
    // in case of rerunning the proggram all of the excisting airports will be deleted.
    if(reRun)
    {
        airports.clear();
    }
    for (const auto &file_itr : fs::directory_iterator(DB_PATH))
        load_db(file_itr.path().filename().string());
}

bool FlightDatabase::load_db(const string &airportCode) noexcept(false)
{
    string airportPathDB(DB_PATH);
    // adding flightsDB directory
    airportPathDB.append("/").append(airportCode); // Add to path .../airportCode // Folder Path
    bool arv = false, dst = false;
    string errors;
    try
    {
        fs::directory_iterator dict_itr(airportPathDB);
        for (const auto &itr : dict_itr)
        {
            try
            {
                if (itr.path().extension().string().compare(".arv") == 0)
                    arv = true;
                else if (itr.path().extension().string().compare(".dst") == 0)
                    dst = true;
                load_flights_to_DB(itr, airportCode);
            }
            catch (const std::logic_error &e)
            {
                errors += string(e.what()).append(" ") += itr.path().filename().string() += " from database\n";
            }
        }
        if (arv == false)
        {
            errors += string("INFO: There are no arrivals to ") += string(airportCode) += " Airport.\n";
        }
        if (dst == false)
        {
            errors += string("INFO: There are no departures to ") += string(airportCode) += " Airport.\n";
        }
    }
    catch (const fs::filesystem_error &e)
    {
        if (e.code().value() == (int)std_error::_File_not_found || e.code().value() == (int)std_error::_Path_not_found)
        {
            errors += string("Error: Airport ") += string(airportCode) += " not found in the Database. \n";
            throw runtime_error(errors);
        }
        else if (e.code().value() == (int)std_error::_Access_denied)
        {
            errors += string("Error: Access Denied to ") += string(airportCode) += " from the Database. \n";
            throw runtime_error(errors);
        }
        else
        {
            errors += string("Error: Unknown error in reading ") += string(airportCode) += " from the Database. \n";
            throw runtime_error(errors);
        }
    }
    // if (!arv || !dst)  Maybe need to remove it
    //     throw runtime_error(errors);
    return true;
}

bool FlightDatabase::load_flights_to_DB(const fs::directory_entry &file, const string &airportCode) noexcept(false)
{
    ifstream inFile;
    inFile.open(file.path().string(), ios::in);
    if (inFile.is_open())
    {
        string dummyString;
        getline(inFile, dummyString); // skip first line
    }
    else
    {
        inFile.close();
        throw logic_error("Error: Something gone wrong in the try to read, ");
    }
    list<Flight> flights;
    try
    {
        getFlightsFromFiles(inFile, flights);
        if (!this->get_airports_names().count(airportCode))
            this->Airports.push_back(Airport(airportCode));
        for (auto &flight : flights)
            if (file.path().extension() == ".arv")
                this->Airports.back().add_flightArv(flight);
            else if (file.path().extension() == ".dst")
                this->Airports.back().add_flightDpt(flight);
            else
            {
                inFile.close();
                throw logic_error("Error: file suffix without handling, ");
            }
        this->Airports.back().changeDataLoadStatus(true);
    }
    catch (const std::exception &e)
    {
        inFile.close();
        throw e;
    }
    inFile.close();
    return true;
}

set<string> FlightDatabase::get_airports_names() const
{
    set<string> airportsNames;
    for (auto &airport : this->Airports)
        airportsNames.insert(airport.get_name());
    return airportsNames;
}

const set<Flight> FlightDatabase::getAirplanes(const string &icao24) const
{
    set<Flight> flights;
    for (auto &airport : this->Airports)
    {
        for (auto &flight : airport.get_flightsArv())
        {
            if (flight == icao24)
                flights.insert(flight);
        }
        for (auto &flight : airport.get_flightsDpt())
        {
            if (flight == icao24)
                flights.insert(flight);
        }
    }
    return flights;
}

const list<Airport> FlightDatabase::get_flights_by_airport_name(const set<string> &airports_names, int ToFromBoth) const
{
    list<Airport> airportsRequired;

    for (auto &airport : this->Airports)
        if (airports_names.count(airport.get_name()))
            airportsRequired.push_back(airport);

    return airportsRequired;
}

const set<Flight> FlightDatabase::getAirplanes(const set<string> &icao24_required) const
{
    set<Flight> flights;
    for (auto &airport : this->Airports)
    {
        for (auto &flight : airport.get_flightsArv())
        {
            if (icao24_required.count(flight.get_icao24()))
                ;
            flights.insert(flight);
        }
        for (auto &flight : airport.get_flightsDpt())
        {
            if (icao24_required.count(flight.get_icao24()))
                ;
            flights.insert(flight);
        }
    }
    return flights;
}

void FlightDatabase::getFlightsFromFiles(ifstream &inFile, list<Flight> &flights) noexcept(false)
{
    if (!inFile.is_open())
        throw logic_error("Error: Something gone wrong in try to reading the file. ");
    string line;
    string icao24, callsign, origin, destination, departure_time, arrival_time;
    while (getline(inFile, line))
    {
        stringstream ss(line);
        getline(ss, icao24, ',');
        getline(ss, departure_time, ',');
        getline(ss, origin, ',');
        getline(ss, arrival_time, ',');
        getline(ss, destination, ',');
        getline(ss, callsign, ',');
        flights.push_front(Flight(icao24, callsign, origin, destination, stoi(departure_time), stoi(arrival_time)));
    }
}

void FlightDatabase::zipDB() noexcept(false)
{
    const string zipFileName = "./flightsDB.zip";
    // Check if FlightsDB folder exists
    if (!fs::is_directory(DB_PATH))
        throw std::runtime_error("Error: flightsDB folder not found.");

    // Create a new zip archive
    zip_t *archive = zip_open(zipFileName.c_str(), ZIP_CREATE | ZIP_TRUNCATE, NULL);
    if (!archive)
        throw std::runtime_error("Error: Failed to create the zip archive.");

    // Iterate over airport folders in FlightsDB
    for (const auto &entry : fs::directory_iterator(DB_PATH))
    {
        if (entry.is_directory())
        {
            string airportCode = entry.path().filename().string();
            string airportFolderPath = "flightsDB/" + airportCode;
            string errors;
            for (const auto &airportEntry : fs::directory_iterator(airportFolderPath))
            {
                string fileName = airportEntry.path().filename().string();
                string filePath = airportFolderPath + "/" + fileName;

                zip_source_t *source = zip_source_file(archive, filePath.c_str(), 0, -1); // create zip_source from file text
                if (!source)                                                              // Error if can't create zip source
                {
                    errors += "Error: create zip source for file " + fileName + '\n';
                    continue;
                }
                zip_int64_t error = zip_file_add(archive, filePath.c_str(), source, ZIP_FL_ENC_UTF_8); // adding the source to the zip file
                if (error == -1)                                                                       // Error if can't add from zip source
                {
                    errors += "Error: add file " + fileName + " to zip archive \n";
                    zip_source_free(source);
                    continue;
                }
            }
            if (errors.length() > 0)
                throw std::runtime_error(errors); // Throw all the errors we collect on the way.
        }
    }
    if (zip_close(archive) == -1)
        throw std::runtime_error("Error: Failed to close the zip archive.");
}
void FlightDatabase::unzipDB() noexcept(false)
{
    const string zipFileName(string(DB_PATH) + ".zip");
    // Check if the zip file exists
    if (!fs::exists(zipFileName))
        throw fs::filesystem_error(zipFileName, make_error_code(errc::no_such_file_or_directory));

    // Open the zip archive
    zip_t *archive = zip_open(zipFileName.c_str(), 0, NULL);
    if (!archive)
        throw runtime_error("Failed to open the zip archive");

    zip_int64_t num_of_zip_entries = zip_get_num_entries(archive, ZIP_FL_UNCHANGED);
    zip_stat_t file_stats;
    int files_failed = 0;
    string errors;

    for (int i = 0; i < num_of_zip_entries; i++)
    {
        int error = zip_stat_index(archive, i, 0, &file_stats);
        zip_file_t *file = zip_fopen_index(archive, i, ZIP_FL_UNCHANGED);
        if (file == nullptr || error == -1)
        {
            ++files_failed;
            continue; // Continue for try next file
        }
        const char *fileName = file_stats.name;
        string fileData;
        fileData.resize(file_stats.size + 1);
        zip_fread(file, fileData.data(), file_stats.size);
        try
        {
            writeStringToFile(fileData, "./" + string(file_stats.name), DB_PREMISSIONS);
        }
        catch (const exception &e)
        {
            errors = errors + e.what() + '\n';
        }
    }
    int status = zip_close(archive);
    if (files_failed > 0 && errors.length() > 0)
        throw runtime_error(errors + "Failed to open " + to_string(files_failed) + "/" + to_string(num_of_zip_entries) + " files from zip archive or get stats of file.");
    else if (files_failed > 0)
        throw runtime_error("Failed to open " + to_string(files_failed) + "/" + to_string(num_of_zip_entries) + " files from zip archive or get stats of file.");
    if (status == -1)
        throw runtime_error("flightsDB has been failed to unzipped in the process !");
}

void FlightDatabase::writeStringToFile(const string &str, const string &fileName, mode_t permissions) // TODO: Change to exceptions
{
    fs::create_directories(fs::path(fileName).parent_path()); // create directories if isn't exist
    ofstream file(fileName, ios::trunc);
    if (file.is_open())
    {
        file << str;
        // Set the premissions using chmod function
        chmod(fileName.c_str(), permissions);
        file.close();
    }
    else
        throw runtime_error("Failed to open the file " + fileName + ".");
}