#include <SDL3/SDL.h>
#include <cstdlib>
#include <ctime>

int main(int argc, char* argv[])
{
	float window_w = 800;
	float window_h = 600;
	int target_framerate_ms = 1000 / 60;       // 16 milliseconds
	int target_framerate_ns = 1000000000 / 60; // 16666666 nanoseconds

	SDL_Window* window = SDL_CreateWindow("E00 - introduction", window_w, window_h, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

	// increase the zoom to make debug text more legible
	// (ie, on the class projector, we will usually use 2)
	{
		float zoom = 1;
		window_w /= zoom;
		window_h /= zoom;
		SDL_SetRenderScale(renderer, zoom, zoom);
	}

	bool quit = false;

	SDL_Time walltime_frame_beg;
	SDL_Time walltime_work_end;
	SDL_Time walltime_frame_end = 0;
	SDL_Time time_elapsed_frame = 0;
	SDL_Time time_elapsed_work;

	SDL_Time time_elapsed_sleep;
	SDL_Time time_elapsed_busywait;

	int delay_type = 0;

	float player_size = 40;
	SDL_FRect player_rect;
	player_rect.w = player_size;
	player_rect.h = player_size;
	player_rect.x = window_w / 2 - player_size / 2;
	player_rect.y = window_h / 2 - player_size / 2;

	// player speed is in pixels/second, not pixels/frame
	float player_speed = 400;

	bool key_held_w = false;
	bool key_held_a = false;
	bool key_held_s = false;
	bool key_held_d = false;

	// 00.2: a few NPCs bouncing around the screen; touching one ends the game
	const int npc_count = 4;
	SDL_FRect npc_rect[npc_count];
	float npc_vel_x[npc_count];
	float npc_vel_y[npc_count];

	srand((unsigned int)time(NULL));
	for(int i = 0; i < npc_count; i++)
	{
		float npc_size = 30;
		npc_rect[i].w = npc_size;
		npc_rect[i].h = npc_size;
		npc_rect[i].x = (float)(rand() % (int)(window_w - npc_size));
		npc_rect[i].y = (float)(rand() % (int)(window_h - npc_size));

		// random direction, fixed speed, so no NPC ends up stuck at 0 velocity
		float angle = (float)(rand() % 360) * (SDL_PI_F / 180.0f);
		float npc_speed = 150;
		npc_vel_x[i] = SDL_cosf(angle) * npc_speed;
		npc_vel_y[i] = SDL_sinf(angle) * npc_speed;
	}

	bool game_over = false;

	SDL_GetCurrentTime(&walltime_frame_beg);
	while(!quit)
	{
		// input
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_EVENT_QUIT:
					quit = true;
					break;
				case SDL_EVENT_KEY_DOWN:
					if(event.key.key >= SDLK_1 && event.key.key < SDLK_6)
						delay_type = event.key.key - SDLK_1;
					switch (event.key.key)
					{
						case SDLK_W: key_held_w = true; break;
						case SDLK_A: key_held_a = true; break;
						case SDLK_S: key_held_s = true; break;
						case SDLK_D: key_held_d = true; break;
					}
					break;
				case SDL_EVENT_KEY_UP:
					switch (event.key.key)
					{
						case SDLK_W: key_held_w = false; break;
						case SDLK_A: key_held_a = false; break;
						case SDLK_S: key_held_s = false; break;
						case SDLK_D: key_held_d = false; break;
					}
					break;
			}
		}

		float delta_time_s = (float)time_elapsed_frame / 1000000000.0f;

		if(!game_over)
		{
			float move_x = (key_held_d ? 1.0f : 0.0f) - (key_held_a ? 1.0f : 0.0f);
			float move_y = (key_held_s ? 1.0f : 0.0f) - (key_held_w ? 1.0f : 0.0f);
			if(move_x != 0.0f && move_y != 0.0f)
			{
				// normalize so diagonal movement isn't faster than axis movement
				move_x *= 0.70710678f;
				move_y *= 0.70710678f;
			}
			player_rect.x += move_x * player_speed * delta_time_s;
			player_rect.y += move_y * player_speed * delta_time_s;

			// 00.2: keep the player fully on screen
			player_rect.x = SDL_clamp(player_rect.x, 0.0f, window_w - player_rect.w);
			player_rect.y = SDL_clamp(player_rect.y, 0.0f, window_h - player_rect.h);

			// 00.2: move NPCs and bounce them off the window edges
			for(int i = 0; i < npc_count; i++)
			{
				npc_rect[i].x += npc_vel_x[i] * delta_time_s;
				npc_rect[i].y += npc_vel_y[i] * delta_time_s;

				if(npc_rect[i].x < 0.0f || npc_rect[i].x > window_w - npc_rect[i].w)
					npc_vel_x[i] = -npc_vel_x[i];
				if(npc_rect[i].y < 0.0f || npc_rect[i].y > window_h - npc_rect[i].h)
					npc_vel_y[i] = -npc_vel_y[i];
				npc_rect[i].x = SDL_clamp(npc_rect[i].x, 0.0f, window_w - npc_rect[i].w);
				npc_rect[i].y = SDL_clamp(npc_rect[i].y, 0.0f, window_h - npc_rect[i].h);

				if(SDL_HasRectIntersectionFloat(&player_rect, &npc_rect[i]))
					game_over = true;
			}
		}

		// clear screen
		// NOTE: `0x` prefix means we are expressing the number in hexadecimal (base 16)
		//       `0b` is another useful prefix, expresses the number in binary
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(renderer);
		
		SDL_SetRenderDrawColor(renderer, 0x3C, 0x63, 0xFF, 0XFF);
		SDL_RenderFillRect(renderer, &player_rect);

		SDL_SetRenderDrawColor(renderer, 0xFF, 0x40, 0x40, 0xFF);
		SDL_RenderFillRects(renderer, npc_rect, npc_count);

		SDL_GetCurrentTime(&walltime_work_end);
		time_elapsed_work = walltime_work_end - walltime_frame_beg;

		if(target_framerate_ns > time_elapsed_work)
		{
			switch(delay_type)
			{
				case 0:
				{
					// busy wait - very precise, but costly
					walltime_frame_end = walltime_work_end;
					while(walltime_frame_end - walltime_frame_beg < target_framerate_ns)
						SDL_GetCurrentTime(&walltime_frame_end);

					time_elapsed_busywait = walltime_frame_end - walltime_work_end;
					time_elapsed_sleep = 0;
					break;
				}
				case 1:
				{
					// simple delay - too imprecise
					// NOTE: `SDL_Delay` gets milliseconds, but our timer gives us nanoseconds! We need to covert it manually
					SDL_Delay((target_framerate_ns - time_elapsed_work) / 1000000);
					SDL_GetCurrentTime(&walltime_frame_end);

					time_elapsed_busywait = 0;
					time_elapsed_sleep = walltime_frame_end - walltime_frame_beg;
					break;
				}
				case 2:
				{
					// delay ns - also too imprecise
					SDL_DelayNS(target_framerate_ns - time_elapsed_work);
					SDL_GetCurrentTime(&walltime_frame_end);

					time_elapsed_busywait = 0;
					time_elapsed_sleep = walltime_frame_end - walltime_frame_beg;
					break;
				}
				case 3:
				{
					// delay precise
					SDL_DelayPrecise(target_framerate_ns - time_elapsed_work);
					SDL_GetCurrentTime(&walltime_frame_end);

					time_elapsed_busywait = 0;
					time_elapsed_sleep = walltime_frame_end - walltime_frame_beg;
					break;
				}
				case 4:
				{
					// custom delay - we use the sleeping delay with an arbitrary margin, then we busywait what's left
					const Uint64 sleep_safety_margin = 1000000; // ie, 1ms
					SDL_Time walltime_sleep_end;
					
					SDL_DelayNS(target_framerate_ns - time_elapsed_work - sleep_safety_margin);
					SDL_GetCurrentTime(&walltime_sleep_end);
					walltime_frame_end = walltime_sleep_end;

					while(walltime_frame_end - walltime_frame_beg < target_framerate_ns)
						SDL_GetCurrentTime(&walltime_frame_end);

					time_elapsed_busywait = walltime_frame_end - walltime_sleep_end;
					time_elapsed_sleep = walltime_sleep_end - walltime_work_end;
					break;
				}
			}
		}

		time_elapsed_frame = walltime_frame_end - walltime_frame_beg;

		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderDebugTextFormat(renderer, 10.0f, 10.0f, "elapsed (frame): %9.6f ms", (float)time_elapsed_frame/(float)1000000);
		SDL_RenderDebugTextFormat(renderer, 10.0f, 20.0f, "elapsed(work   : %9.6f ms", (float)time_elapsed_work/(float)1000000);
		SDL_RenderDebugTextFormat(renderer, 10.0f, 30.0f, "delay type: %d (change with 1-5)", delay_type + 1);

		SDL_RenderDebugTextFormat(renderer, 10.0f, 50.0f, "time spent sleeping   : %9.6f ms", (float)time_elapsed_sleep/(float)1000000);
		SDL_RenderDebugTextFormat(renderer, 10.0f, 60.0f, "time spent busywaiting: %9.6f ms", (float)time_elapsed_busywait/(float)1000000);

		if(game_over)
		{
			SDL_SetRenderDrawColor(renderer, 0xFF, 0x40, 0x40, 0xFF);
			SDL_RenderDebugTextFormat(renderer, window_w / 2 - 40, window_h / 2, "GAME OVER");
		}


		// render
		SDL_RenderPresent(renderer);
		
		walltime_frame_beg = walltime_frame_end;
	}

	// NOTE: we created a bunch of resources (window, renderer). Should we explicitely destroy them here?
	//       it's actually not a trivial question!
	
	return 0;
};
