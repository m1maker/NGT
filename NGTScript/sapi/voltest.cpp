#include<iostream>
#include"SpeechEngine.h"
int main()
{
SpeechEngine* sp=new SpeechEngine();
sp->speak_wait("speaking wait...");
std::cout << "Volume: " << sp->getVolume() << '\n';
delete sp;
sp=nullptr;
return 0;
}