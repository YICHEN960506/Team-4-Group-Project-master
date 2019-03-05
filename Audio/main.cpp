#include <iostream>
#include "fmod.hpp"
#pragma comment(lib,"fmod_vc.lib")
#include "AudioEngine.h"
#include <windows.h>
#include <fstream>


using namespace std;

inline bool exists_test0(const std::string& name) {
	ifstream f(name.c_str());
	return f.good();
}
int main()
{
	
	AudioEngine hi;
	hi.Init();
	if (exists_test0("../Audio/Sound/Sarias Song Extended 10 Minutes.mp3")) {

		hi.LoadSound("../Audio/Sound/Sarias Song Extended 10 Minutes.mp3", true, false, false);
			hi.PlaySounds("../Audio/Sound/Sarias Song Extended 10 Minutes.mp3", Audio::Vector3(0, 0, 0), 1.0f);

			int i = 0;
			do
			{
				hi.Update();
				i++;
				Sleep(10);
			} while (i < 10000);
	}
	

	return 0;
}