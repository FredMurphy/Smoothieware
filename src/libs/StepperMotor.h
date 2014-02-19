/*
      This file is part of Smoothie (http://smoothieware.org/). The motion control part is heavily based on Grbl (https://github.com/simen/grbl).
      Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
      Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
      You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef STEPPERMOTOR_H
#define STEPPERMOTOR_H

#include "libs/Hook.h"
#include "Pin.h"
#include "Actuator.h"

class StepTicker;
class Hook;

class StepperMotor : public Actuator {
    public:
        StepperMotor();
        StepperMotor(Pin& step, Pin& dir, Pin& en);

        // Called a great many times per second, to step if we have to now
        inline void tick() {
            // increase the ( fixed point ) counter by one tick 11t
            fx_counter += (uint32_t)(1<<16);

            // if we are to step now 10t
            if (fx_counter >= fx_ticks_per_step)
                step();
        };

        void step();
        inline void unstep() { step_pin.set(0); };

        inline void enable(bool state) { en_pin.set(!state); };

        void move_finished();

        void move( bool direction, unsigned int steps );
        void stop(void);

        void signal_move_finished();
        void set_speed( float speed );
        void update_exit_tick();
        void pause();
        void unpause();

        void change_steps_per_mm(float);
        void change_last_milestone(float);

        int  steps_to_target(float);

        template<typename T> void attach( T *optr, uint32_t ( T::*fptr )( uint32_t ) ){
            Hook* hook = new Hook();
            hook->attach(optr, fptr);
            this->end_hook = hook;
        }

        template<typename T> void attach_signal_step(uint32_t step, T *optr, uint32_t ( T::*fptr )( uint32_t ) ){
            this->step_signal_hook->attach(optr, fptr);
            this->signal_step_number = step;
            this->signal_step = true;
        }

        Hook* end_hook;
        Hook* step_signal_hook;

        bool signal_step;
        uint32_t signal_step_number;

        StepTicker* step_ticker;
        Pin step_pin;
        Pin dir_pin;
        Pin en_pin;

        float steps_per_second;

        bool paused;

        float steps_per_mm;

        int32_t last_milestone_steps;

        uint32_t steps_to_move;
        uint32_t stepped;
        uint32_t fx_counter;
        uint32_t fx_ticks_per_step;

        bool     direction;

        //bool exit_tick;
        bool remove_from_active_list_next_reset;

        bool is_move_finished; // Whether the move just finished
};

#endif

