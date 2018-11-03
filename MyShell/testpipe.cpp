#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <string>
#include <sys/wait.h>
#include <fcntl.h>
#include <sstream>
#include <iomanip>
using namespace std;

int IO_redirection(vector<string> &t, vector<string> &a);
void cd_command(vector<string> &t, vector<string> &d);
void parse_pipedlines(vector<string> &p, vector<string> &t, int i);
int pipehelper(int in, int out, vector<char*> &a);

int main(int arg, char **argv)
{
    char command[100];
    string c;
    stringstream b;

   
    cout << "$MyShell:  ";
    cin.getline(command,100);
    vector<string> tokens;
    vector<string> pipelines;
    vector<string> arglist;
    vector<char*> argvv;
    vector<string> directories;
    vector<pid_t> pids;
    char * token;

    for(int i = 0; i < strlen(command); i++)
    {
        if(command[i] == '>')
            b << " " << command[i] << " ";  
        else if(command[i] == '<')
            b << " " << command[i] << " ";  
        else if(command[i] == ' ')               //make sure symbols and commands are seperated by spaces
            b << " ";
        else   
            b << command[i];
    }
    c = b.str();
    strcpy(command, &c[0]);


    
    while(strcmp(command, "exit") != 0)
    {

        token = strtok(command,"|");
    
        while(token != NULL)
        {
            string temp = token;
            pipelines.push_back(temp);                 // parse command and seperate by pipe symbols
            token = strtok(NULL,"|");
        }

        //cout << pipelines.size();
        if(pipelines.size() == 1) // no pipes
        {
            if(!pids.empty() && pids.size() % 2 == 0)
            {
                for(int i = 0; i < pids.size(); i++)  //chekc background processes
                {
                    pid_t pc;
                    int status;
                    pc = waitpid(pids[i],&status,WNOHANG);
                        if(pc > 0)
                            wait(&pids[i]);
                }
            }
        
            int background = 0;
            parse_pipedlines(pipelines, tokens, 0);
            for(int i = 0; i < tokens.size(); i++)
            {
                if(tokens[i] == "&")
                    background = 1;
            }   
                //---------------------------------------------------
            if(tokens[0] == "cd") // Check cd command
            {
                cd_command(tokens, directories);
            }
                //---------------------------------------------------
            else
            {                
                int pid = fork();
                if(pid == 0)  // child process                              
                {
                    
                    IO_redirection(tokens, arglist);
                   
                    
    
                    for(int i = 0; i < arglist.size(); i++)
                    {
                        argvv.push_back(const_cast<char*>(arglist[i].c_str()));
                        
                    }
                   
                    if(background == 1)
                    {
                        //setpgid(0,0);
                        argvv.pop_back();
                        pids.push_back(getppid());
                    }
                    argvv.push_back(NULL);
                
                    int e = execvp(argvv[0],&argvv[0]);
                    if(e == -1)
                        cout << "**Not a valid command**" << endl;

                }
                else 
                {
                    if(background == 0)
                        wait(NULL);
                    //else
                        //wait(0);
                }
            }    
        }
        else
        {
            vector<string> leftlist;
            int **fds;
            for(int i = 0; i < pipelines.size(); i++)
                fds[i] = new int[2];
            int in, fd[2];
            in = 0;
            int leftStart = 0;    // This variable will keep track of the beginning of each pipedline block that has been parsed by whitespace
            for(int j = 0; j < pipelines.size()-1; j++)
            {
                    pipe(fd);

                    parse_pipedlines(pipelines, tokens, j); // parse pipedline and seperate by whitespace
            
                    for(int i = leftStart; i < tokens.size(); i++)
                    {
                        leftlist.push_back(tokens[i]);          //fill argument list for execvp function
                    }
                    
                    IO_redirection(leftlist,arglist);
                    
                    for(int i = 0; i < leftlist.size(); i++)
                        argvv.push_back(const_cast<char*>(arglist[i].c_str()));
                    
                    argvv.push_back(NULL);

                    pipehelper(in,fd[1], argvv);

                    close(fd[1]);
                    in = fd[0];
                   
            
                
                leftStart = tokens.size();
                leftlist.clear();
                arglist.clear();
                argvv.clear();
            }

                if(in != 0)
                    dup2(in,0);
   
                parse_pipedlines(pipelines, tokens, pipelines.size() - 1);
                for(int m = leftStart; m < tokens.size(); m++)
                    leftlist.push_back(tokens[m]);

                IO_redirection(leftlist,arglist);
                

                for(int i = 0; i < leftlist.size(); i++)
                {
                    argvv.push_back(const_cast<char*>(arglist[i].c_str()));
                }
            
    
                argvv.push_back(NULL);
           
                execvp(argvv[0], &argvv[0]);
           
    
        }
    

        cout << "$MyShell:  ";
        tokens.clear();
        pipelines.clear();
        arglist.clear();
        argvv.clear();
        cin.getline(command,100);
        stringstream b;
        c = command;
        for(int i = 0; i < strlen(command); i++)
        {
            if(command[i] == '>')
                b << " " << command[i] << " ";
            else if(command[i] == '<')
                b << " " << command[i] << " ";
            else if(command[i] == ' ')
                b << " ";
            else   
                b << command[i];
        }
        c = b.str();
        strcpy(command, &c[0]);

    }
    return 0;
}

// break up an index of pipeline by spaces, dealing with quoted commands as well
void parse_pipedlines(vector<string> &p, vector<string> &t, int i)
{

    stringstream b;
    string temp = p[i];
    int last = temp.length() - 1;
    for(int j = 0; j < temp.length(); j++)
    {
            
        if(temp[j] == ' ' || (j == temp.length() -1))
        {
            if(j == last)
            {
                if(!isspace(temp[last]))
                    b << temp[j];
            }
            string temp1 = b.str();
            t.push_back(temp1);
            b.str("");
        }
        else if(temp[j] == '\'')
        {
            b << temp[j+1];
            int m = j + 2;
            while(temp[m] != '\'')
            {
                b << skipws <<temp[m] << skipws;
                m++;
            } 
            j = m+1;
            string temp2 = b.str();
            t.push_back(temp2);
            b.str("");   
        }
        else
        {
            b << temp[j];
        }
    }   
    for(vector<string>::iterator it = t.begin() ; it != t.end(); ++it) //clear any whitespaces added to vector
    {
        if(*it == "")
            t.erase(it);
    }

}


// Handles IO check and redirection
int IO_redirection(vector<string> &t, vector<string> &a)
{
    for(int i = 0; i < t.size(); i++) // search for I/O redirection
        {
            if(t[i] == ">")
            {
                const char* temp = const_cast<char*>(t[i+1].c_str());
                int fd = open(temp, O_CREAT|O_WRONLY,S_IRUSR|S_IWUSR);
                if(fd < 0)
                    perror("open");
                dup2(fd,1);
                break;
            }
            else if(t[i] == "<")
            {
                const char* temp = const_cast<char*>(t[i+1].c_str());
                int fd = open(temp, O_CREAT|O_RDONLY,S_IRUSR|S_IWUSR);
                if(fd < 0)
                    perror("open");
                dup2(fd,0);
                break;
            }
            a.push_back(t[i]); // fill arglist with tokens left of special token
        }
}


// Handles cd command
void cd_command(vector<string> &t, vector<string> &d)
{
    int c;
    cout << t[1];
    if(t[1] == "-")
    {
            const char * previous = "..";                      
            c = chdir(previous);
       
    }
    else
    {
        const char* temp = const_cast<char*>(t[1].c_str());
        c = chdir(temp);
        d.push_back(t[1]);
        cout << d.back();
    }

}


int pipehelper(int in, int out, vector<char*> &a)
{
    pid_t pid;
    if(fork() == 0)
    {
        if(in != 0)
        {
            dup2(in,0);
            close(in);
        }

        if(out != 1)
        {
            dup2(out, 1);
            close(out);
        }
        return execvp(a[0], &a[0]);
    }
return pid;
}
