#ifndef DATE_H
#define DATE_H
class Timer
{
    private:
        // The clock time when the timer started
        int startTicks;

        // The ticks stored when the timer was paused
        int pausedTicks;

        // The timer status
        bool paused;
        bool started;

    public:
        // Constructor
        Timer();

        // Various actions
        void start();
        void stop();
        void pause();
        void unpause();

        // Get the timer's time
        int get_ticks();

        // Check the status of the timer
        bool is_started();
        bool is_paused();
};
#endif
