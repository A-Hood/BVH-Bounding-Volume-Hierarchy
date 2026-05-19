#include "Application.h"

/*
 * Create a "Dynamic" flag for each collider, store the dynamic colliders
 * as these are the colliders that will need to be checked against the others.
 * The static colliders will never move so do not need to update or need to check with other colliders.
 */

int main() {
    Application app;
    app.CreateApplication();

    app.Run();

    // Run has ended, close
    app.Close();

    return 0;
}