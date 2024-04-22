
#include "player.h"
#include "fun_piano.h"

int main()
{
	ADPCMSound sound(fun_piano_bin,fun_piano_bin_len);
	Player::instance().play(sound);
}
