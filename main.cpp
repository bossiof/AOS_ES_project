
#include "player.h"
#include "reproducible_audio.h"

int main()
{
	ADPCMSound sound(reproducible_audio_bin,reproducible_audio_bin_len);
	Player::instance().play(sound);
}
