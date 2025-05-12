#pragma once

#include <Arduino.h>

class Timer
{
private:
    unsigned long targetMicros; // Durée restante en microsecondes
    unsigned long lastMicros;   // Dernier instant mesuré
    bool active;                // Timer actif ou non

public:
    // Constructeur
    Timer()
    {
        targetMicros = 0;
        lastMicros = 0;
        active = false;
    }

    // Démarrer le timer avec un délai en millisecondes
    void start(unsigned long delayMillis)
    {
        targetMicros = delayMillis * 1000UL; // Convertir en microsecondes
        lastMicros = micros();
        active = true;
    }

    // Mettre à jour l'état du timer, retourne le temps restant en microsecondes
    unsigned long update()
    {
        if (!active)
        {
            return 0;
        }

        unsigned long now = micros();
        unsigned long elapsed = now - lastMicros;
        lastMicros = now;

        if (elapsed >= targetMicros)
        {
            active = false;
            targetMicros = 0;
            return 0;
        }
        else
        {
            targetMicros -= elapsed;
            return targetMicros;
        }
    }

    // Savoir si le timer est toujours actif
    bool isRunning() const
    {
        return active;
    }

    // Remettre complètement le timer
    void reset()
    {
        targetMicros = 0;
        lastMicros = micros();
        active = false;
    }
};
