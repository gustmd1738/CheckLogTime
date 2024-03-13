#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <ctime>
#include <iomanip>

using namespace std;


string filename = "C:\\Users\\USER\\Desktop\\소프트센\\장비\\VRI-P018R\\VRI-P018R 운영프로그램 Process delay Issue_20240312\\PC1\\VxWorks_develop_2024-03-10.log"; // 파일 경로를 여기에 입력하세요.
int delayTime = 60; // 초단위
bool fathfail = false;


// 함수 선언
vector<string> findSuccessTimes(const string& filename);
float convertToTime(const string& timeString);
void checkForErrors(const vector<string>& successTimes);

int main() {

    vector<string> successTimes = findSuccessTimes(filename);

    if (!fathfail)
    {
        // 에러 체크
        checkForErrors(successTimes);
    }

    return 0;
}

vector<string> findSuccessTimes(const string& filename) {
    vector<string> successTimes;
    ifstream file(filename);

    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            size_t found = line.find("Success to get cell ready signal");
            if (found != string::npos) {
                // 시간 추출
                string successTime;
                bool timeWrite = false;
                for (size_t i = 0; i <= line.size(); i++)
                {
                    if (line[i] == '|' && !timeWrite)
                    {
                        timeWrite = true;
                        i++;
                        continue;
                    }
                    else if (line[i] == '|' && timeWrite)
                    {
                        break;
                    }

                    if(timeWrite)
                        successTime += line[i];
                }

                successTimes.push_back(successTime);
            }
        }
        file.close();
    }
    else {
        cerr << "해당경로의 파일을 열 수 없습니다." << endl;
        fathfail = true;
    }

    return successTimes;
}

float convertToTime(const string& timeString) {
    int hour = 0;
    int min = 0;
    int sec = 0;
    float ms = 0.f;
    
    int num = -1;
    string temp = "";
    for (size_t i = 0; i < timeString.size(); i++)
    {
        if (timeString[i] == ':' || timeString[i] == '.' || timeString[i] == ' ')
        {
            switch (num)
            {
            case -1:
                if (timeString[i] == ' ')
                {
                    num = 0;
                    temp = "";
                }
                break;
            case 0:
                hour = stoi(temp);
                num++;
                temp = "";
                break;
            case 1:
                min = stoi(temp);
                num++;
                temp = "";
                break;
            case 2:
                sec = stoi(temp);
                num++;
                temp = "";
                break;
            case 3:
                ms = stof(temp)/1000.f;
                num++;
                temp = "";
                break;
            default:
                break;
            }

        }
        else
        {
            temp += timeString[i];
            if (i == timeString.size() - 1 && num == 3)
            {
                ms = stof(temp)/1000.f;
                num++;
                temp = "";
            }
        }
        
    }

    float result = hour * 3600;
    result += min * 60;
    result += sec;
    result += ms;

    return result;
}

void checkForErrors(const vector<string>& successTimes) {
    int total = successTimes.size() -1;
    int error = 0;

    for (size_t i = 0; i < successTimes.size() - 1; ++i) {
        float currentTime = convertToTime(successTimes[i]);
        float nextTime = convertToTime(successTimes[i + 1]);
        float diffInSeconds = nextTime - currentTime;

        if (diffInSeconds >= delayTime) {
            cerr << "다음측정시간까지 " << diffInSeconds << "초가 소요되었습니다." << endl;
            cerr << "이전 시간: " << successTimes[i] << endl;
            cerr << "다음 시간: " << successTimes[i + 1] << endl;
            error++;
        }
    }

    cout << "\n 총 " << total << "번의 측정동안 측정시간이 "<< delayTime << "초이상 소요된 횟수는 " << error << "번입니다. " << endl;


}