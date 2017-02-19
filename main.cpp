//
//  main.cpp
//  ProcessShell
//
//  Created by Scott Obray on 2/13/17.
//  Copyright © 2017 Scott Obray. All rights reserved.
//

#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <chrono>
#include <algorithm>


std::chrono::duration<double> timeSpent;
double totalTime = 0.0;
std::vector<std::string> commandHistory;
bool running = true;

void ptime()
{
    std::cout << "Time spent executing child processes: "
    << std::chrono::duration_cast<std::chrono::seconds>(timeSpent).count()
    << " seconds, " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpent).count()
    << " miliseconds, and " << std::chrono::duration_cast<std::chrono::microseconds>(timeSpent).count()
    << " microscends." << std::endl;
}

void history(std::string temp)
{
    if (!temp.compare("history"))
    {
        for (int i = 0; i < (int)commandHistory.size(); i++)
        {
            std::cout << commandHistory[i] << std::endl;
        }
    }
    else
    {
        std::string historyNumber;
        std::vector<std::string> words;
        std::vector<char*> args;
        for (int i = 2; i < (int)temp.size(); i++)
        {
            historyNumber.push_back(temp[i]);
        }
        int value = atoi(historyNumber.c_str());
        if (value > (int)commandHistory.size())
        {
            std::cout << "Not enough entries in history list.\n";
        }
        else
        {
            auto cur = commandHistory[value].begin();
            while (cur!= commandHistory[value].end())
            {
                auto next = std::find(cur, commandHistory[value].end(), ' ');
                words.emplace_back(cur, next);
                cur = std::find(cur, commandHistory[value].end(), ' ');
                if (cur == commandHistory[value].end())
                {
                    break;
                }
                ++cur;
            }
            
            
            for (int i = 0; i < (int) words.size(); i++)
            {
                args.push_back((char*)words[i].c_str());
            }
            
            auto start = std::chrono::steady_clock::now();
            auto pid = fork();
            if (pid < 0)
            {
                perror("error");
                exit(EXIT_FAILURE);
            }
            if (pid == 0)
            {
                execvp(words[0].c_str(), args.data());
            }
            else
            {
                int status;
                waitpid(pid, &status, 0);
                auto end = std::chrono::steady_clock::now();
                timeSpent = end - start;
                totalTime += timeSpent.count();
            }
        }
    }
    
    
}

int main()
{
    
    
    while (running)
    {
        std::string line;
        std::vector<std::string> words;
        std::vector<char*> args;
        
        std::cout << "[cmd]: ";
        std::getline(std::cin, line);
        commandHistory.push_back(line);
        
        if (!line.compare("exit"))
        {
            running = false;
            std::cout << "Exiting Program\n";
            exit(EXIT_FAILURE);
            break;
        }
        else if (!line.compare("ptime"))
        {
            ptime();
        }
        else if (!line.compare("history") || line[0] == '^')
        {
            history(line);
        }
        else
        {
            auto cur = line.begin();
            while (cur!= line.end())
            {
                auto next = std::find(cur, line.end(), ' ');
                words.emplace_back(cur, next);
                cur = std::find(cur, line.end(), ' ');
                if (cur == line.end())
                {
                    break;
                }
                ++cur;
            }
            
            
            for (int i = 0; i < (int) words.size(); i++)
            {
                args.push_back((char*)words[i].c_str());
            }
            
            auto start = std::chrono::steady_clock::now();
            auto pid = fork();
            if (pid < 0)
            {
                perror("error");
                exit(EXIT_FAILURE);
            }
            if (pid == 0)
            {
                execvp(words[0].c_str(), args.data());
            }
            else
            {
                int status;
                waitpid(pid, &status, 0);
                auto end = std::chrono::steady_clock::now();
                timeSpent = end - start;
                totalTime += timeSpent.count();
            }
        }
    }
}
