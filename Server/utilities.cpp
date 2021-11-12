#include "utilities.h"

void utilities::save_data_to_path(char *request_msg, char *file_path) 
{
	std::string s = std::string(request_msg);
	std::string path = "public" + std::string(file_path);
	std::ofstream MyFile(path);
	s = s.substr(s.find(CONTENT_LENGTH) + std::string(CONTENT_LENGTH).length());
    int cLen = stoi(s.substr(0,s.find("\n")));
    s = s.substr(s.find("\n")+3, cLen-1);
	printf("Writitng to path %s\n",path.c_str());
	MyFile << s; // Write to the file
	MyFile.close(); // Close the file
}

std::string utilities::read_data_from_path(char *file_path) 
{
	std::string path = "public" + std::string(file_path);
	printf("Reading from path %s\n",path.c_str());
	std::string totText;
	std::ifstream MyReadFile(path);
	if (MyReadFile.good()) {
		std::string myText; 
		totText = "";
		while (getline (MyReadFile, myText)){
			totText += myText + "\n"; // Write to the file
		}
		if (totText.length() >= 1)
			totText.substr(0, totText.length()-1);
		
		MyReadFile.close(); // Close the file

	}
	else {
		totText = "";
	}
	return totText;
}

struct messege_content utilities::request_processing(char command[]) 
{
/*  GET /file-path HTTP/1.1
	Host: host-name:(port-number)  */
	/* 0 GET
	1 /file-path
	2 HTTP/1.1
	3 Host
	4 host-name
	5 (port-number) */

	struct messege_content cmd;
	strcpy(cmd.request_msg, command);
	
	int lArgs = 0;
    char *args[100];
    char *token = strtok(command, " ");
    while (token != NULL) { 
        args[lArgs++] = token;
        token = strtok(NULL, " |\n|:"); 
    } 
    args[lArgs] = NULL;
	strcpy(cmd.request, args[0]);
	strcpy(cmd.file_path, args[1]);
	strcpy(cmd.host_name, args[4]);
	strcpy(cmd.port_number, args[5]);
	return cmd;
}
