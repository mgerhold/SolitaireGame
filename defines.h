#ifndef DEFINES_H
#define DEFINES_H

#define BOARD_DIMENSION 7

#define SCREEN_WIDTH 800

#define SCREEN_HEIGHT 600

#define BOARD_RADIUS ((float) SCREEN_HEIGHT * 0.48f)

#define INNER_FIELD_HALF_WIDTH (BOARD_RADIUS * 0.6f)

#define OUTER_RING_RADIUS (BOARD_RADIUS * 0.9f)

// distance between fields
#define FIELD_MARGIN (2.0f * INNER_FIELD_HALF_WIDTH / (BOARD_DIMENSION - 1))

#define FIELD_RADIUS (BOARD_RADIUS / BOARD_DIMENSION / 2.0f)

#define OUTER_RING_BALL_MIN_DISTANCE (FIELD_RADIUS * 2.0f)

#define OUTER_RING_BALL_MAX_DISTANCE (FIELD_RADIUS * 3.0f)

#define BOARD_COLOR ((Color){ 135, 62, 35, 255 })

#define BALL_COLOR WHITE

#define EMPTY_FIELD_COLOR ((Color){ 33, 19, 13, 255 })

#define SELECTED_BALL_COLOR RED

#define POSSIBLE_MOVE_TARGET_COLOR YELLOW

#define SCORE_CAP (BOARD_DIMENSION * BOARD_DIMENSION - (BOARD_DIMENSION / 3 * 2) * 4 - 1)

#endif
