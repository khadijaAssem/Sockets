#include "utilities.h"

std::string utilities::read_from_file (std::string path)
{
    std::ifstream MyReadFile(path);
    if (MyReadFile.good()) {
        std::string myText; 
        std::string totText;
        totText = "";
        while (getline (MyReadFile, myText))
            totText += myText  + "\n"; // Write to the file
        if (totText.length() >= 1)
			totText.substr(0, totText.length()-1);
        MyReadFile.close(); // Close the file

        return totText;
    }
    printf("FILE NOT FOUND: %s\n", path.c_str());
    return "";
}

void utilities::save_recieved_content(char *buffer, char *file_path) 
{
    std::string s = buffer;
    if (strcmp(buffer, NOTFOUND_RESPONSE)) {
        std::string path = "local/" + std::string(file_path);
        std::ofstream MyFile(path);
        s = s.substr(s.find(CONTENT_LENGTH) + std::string(CONTENT_LENGTH).length());
        int cLen = stoi(s.substr(0,s.find("\n")));
        s = s.substr(s.find("\n")+3, cLen-1);
        printf("Writitng to path %s\n",path.c_str());
        MyFile << s; // Write to the file
        MyFile.close(); // Close the file
    }
}

struct messege_content utilities::request_preprocessing(char command[])
{
    // client_get file-path host-name (port-number)
    // client_post file-path host-name (port-number)

    int lArgs = 0;
    char *args[5];
    char *token = strtok(command, " "); 
    while (token != NULL) { 
        args[lArgs++] = token;
        token = strtok(NULL, " "); 
    } 

    args[lArgs] = NULL;
    struct messege_content cmd;
    char extra_data[MAXDATASIZE];
    char request[MAXDATASIZE];
    char *host_name = args[2], *port_number = args[3];
    char *file_path = args[1];

    if (strcmp(args[0] ,"client_get") == 0) {
        strcpy(cmd.request, GET_REQUEST);
        strcpy(request ,"GET /");
    }
    else if (strcmp(args[0] ,"client_post") == 0) {
        strcpy(cmd.request, POST_REQUEST);
        strcpy(request ,"POST /");
        strcpy(extra_data, (char *)read_from_file(("local/" + std::string(file_path))).c_str());
    }

    strcat(request,file_path); strcat(request," HTTP/1.1\n"); strcat(request,"Host: ");
    strcat(request,host_name); strcat(request,":"); strcat(request,port_number); strcat(request,"\n");

    if (strcmp(cmd.request ,POST_REQUEST) == 0) {
        strcat(request, CONTENT_LENGTH);
        int length_extra_data = std::string(extra_data).length();
        strcat(request, std::to_string(length_extra_data).c_str());
        strcat(request,ENDREQUEST); strcat(request,ENDREQUEST);
        strcat(request, extra_data);
    }
    strcat(request,ENDREQUEST);

    strcpy(cmd.file_path, file_path);
    strcpy(cmd.host_name, host_name);
    strcpy(cmd.port_number, port_number);
    strcpy(cmd.request_msg, request);

    return cmd;
}
