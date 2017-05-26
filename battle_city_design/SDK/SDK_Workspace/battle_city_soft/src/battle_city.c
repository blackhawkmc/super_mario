#include "battle_city.h"
#include "map.h"
#include "xparameters.h"
#include "xil_io.h"
#include "xio.h"

/*
* GENERATED BY BC_MEM_PACKER
* DATE: Wed Jul 08 21:00:48 2015
*/

// ***** 16x16 IMAGES *****

#define IMG_16x16_cigle			0x00FF //2
#define IMG_16x16_coin			0x013F //5
#define IMG_16x16_crno			0x017F //0
#define IMG_16x16_enemi1		0x01BF //4
#define IMG_16x16_mario			0x01FF //1
#define IMG_16x16_plavacigla	0x023F //3


// ***** MAP *****

#define MAP_BASE_ADDRESS			0x027F


#define MAP_X							0
#define MAP_X2							640
#define MAP_Y							4
#define MAP_W							64
#define MAP_H							56

#define REGS_BASE_ADDRESS               ( MAP_BASE_ADDRESS + MAP_WIDTH * MAP_HEIGHT )



#define BTN_DOWN( b )                   ( !( b & 0x01 ) )
#define BTN_UP( b )                     ( !( b & 0x10 ) )
#define BTN_LEFT( b )                   ( !( b & 0x02 ) )
#define BTN_RIGHT( b )                  ( !( b & 0x08 ) )
#define BTN_SHOOT( b )                  ( !( b & 0x04 ) )

#define TANK1_REG_L                     8
#define TANK1_REG_H                     9
#define TANK_AI_REG_L                   4
#define TANK_AI_REG_H                   5
#define TANK_AI_REG_L2                  6
#define TANK_AI_REG_H2                  7
#define TANK_AI_REG_L3                  2
#define TANK_AI_REG_H3                  3
#define TANK_AI_REG_L4                  10
#define TANK_AI_REG_H4                  11
#define TANK_AI_REG_L5                  12
#define TANK_AI_REG_H5                  13
#define TANK_AI_REG_L6                  14
#define TANK_AI_REG_H6                  15
#define TANK_AI_REG_L7                  16
#define TANK_AI_REG_H7                  17
#define BASE_REG_L						0
#define BASE_REG_H	                    1

int  prvi1 = 1;
int	 prvi2 = 1;
int  prvi3 = 1;


typedef enum {
    b_false,
    b_true
} bool_t;

typedef enum {
    DIR_LEFT,
    DIR_RIGHT,
    DIR_UP,
    DIR_DOWN
} direction_t;

typedef struct {
    unsigned int    x;
    unsigned int    y;
    direction_t dir;
    unsigned int    type;

    bool_t          destroyed;


    unsigned int	reg_l;
    unsigned int	reg_h;
} characters;

characters mario = {
    227,	                        // x
    431, 		                     // y
    DIR_RIGHT,              		// dir
    IMG_16x16_mario,  			// type

    b_false,                		// destroyed

    TANK1_REG_L,            		// reg_l
    TANK1_REG_H             		// reg_h
};

characters enemie = {
    331,						// x
    431,						// y
    DIR_LEFT,              		// dir
    IMG_16x16_enemi1,  		// type

    b_false,                		// destroyed

    TANK_AI_REG_L,            		// reg_l
    TANK_AI_REG_H             		// reg_h
};

characters coin1 = {
    454,						// x
    304,						// y
    DIR_LEFT,              		// dir
    IMG_16x16_coin,  		// type

    b_false,                		// destroyed

    TANK_AI_REG_L2,            		// reg_l
    TANK_AI_REG_H2             		// reg_h
};

characters coin2  = {
    469,						// x
    304,						// y
    DIR_LEFT,              		// dir
    IMG_16x16_coin,  		// type

    b_false,                		// destroyed

    TANK_AI_REG_L3,            		// reg_l
    TANK_AI_REG_H3             		// reg_h
};

characters coin3  = {
    484,						// x
    304,						// y
    DIR_LEFT,              		// dir
    IMG_16x16_coin,  		// type

    b_false,                		// destroyed

    TANK_AI_REG_L4,            		// reg_l
    TANK_AI_REG_H4             		// reg_h
};



unsigned int rand_lfsr113( void )
{
	static unsigned int z1 = 12345, z2 = 12345 ;
	unsigned int b;

	b  = ( ( z1 << 6 ) ^ z1 ) >> 13;
	z1 = ( ( z1 & 4294967294U ) << 18 ) ^ b;
	b  = ( ( z2 << 2 ) ^ z2 ) >> 27;
	z2 = ( ( z2 & 4294967288U ) << 2) ^ b;


	return ( z1 ^ z2 );
}

static void chhar_spawn( characters * chhar )
{
	Xil_Out32( XPAR_BATTLE_CITY_PERIPH_0_BASEADDR + 4 * ( REGS_BASE_ADDRESS + chhar->reg_l ),  (unsigned int)0x8F000000  | (unsigned int)chhar->type );
	Xil_Out32( XPAR_BATTLE_CITY_PERIPH_0_BASEADDR + 4 * ( REGS_BASE_ADDRESS + chhar->reg_h ),  (chhar->y << 16) | chhar->x );
}

static void map_update()
{
    int x, y;
    long int addr;
    int map_base_address = MAP_BASE_ADDRESS;

    for(y = 0; y < MAP_HEIGHT; y++) {
    	for(x = 0; x < MAP_WIDTH; x++) {
    		addr = XPAR_BATTLE_CITY_PERIPH_0_BASEADDR + 4 * ( map_base_address + y*MAP_WIDTH+x);
			switch(map1[y][x+120]){
			case 0 :
				Xil_Out32(addr, IMG_16x16_crno);
				break;
			case 1 :
				Xil_Out32( addr, IMG_16x16_mario);
				break;
			case 2 :
				Xil_Out32( addr, IMG_16x16_cigle);
				break;
			case 3 :
				Xil_Out32( addr, IMG_16x16_plavacigla);
				break;
			case 4 :
				Xil_Out32( addr, IMG_16x16_enemi1);
				break;
			case 5 :
				Xil_Out32(addr, IMG_16x16_coin);
				break;
			default :
				Xil_Out32( addr, IMG_16x16_crno);
				break;
			}
		}
    }
}

static void map_reset( unsigned char * map ) {

    unsigned int i;

    for( i = 0; i <= 20; i += 2 ) {
    	Xil_Out32( XPAR_BATTLE_CITY_PERIPH_0_BASEADDR + 4 * ( REGS_BASE_ADDRESS + i ), (unsigned int)0x0F000000 );
    }

}

static bool_t jump(unsigned char * map, characters * mario, direction_t dir, int upOrDown ){

	unsigned int    x;
	unsigned int    y;
/*
	map_entry_t *   tl;
	map_entry_t *   tc;
	map_entry_t *   tr;
	map_entry_t *   cl;
	map_entry_t *   cc;
	map_entry_t *   cr;
	map_entry_t *   bl;
	map_entry_t *   bc;
	map_entry_t *   br;
	*/

    unsigned char *   mario_left;
    unsigned char *   mario_right;
    unsigned char *   mario_center;
    unsigned char *   coin_left;
    unsigned char *   coin_right;
    unsigned char *   coin_center;
    unsigned char *   brick_left;
    unsigned char *   brick_right;
    unsigned char *   brick_center;

	if( mario->x > (( MAP_X + MAP_WIDTH ) * 16 - 16 ) ||
		mario->y > ( MAP_Y + MAP_HEIGHT ) * 16 - 16 ) {
		return b_false;
	}

	x = mario->x;
	y = mario->y;

	// Make sure that coordinates will stay within map boundaries after moving.
	if( dir == DIR_LEFT ) {
		if( x > MAP_X * 16 / 2){
			if(upOrDown == 1){   //up left
				x-=20;
				y-=40;
			}else{     //down left
				x-=20;
				y+=40;
			}

		}
	} else if( dir == DIR_RIGHT ) {
		if( x < ( (MAP_X + MAP_WIDTH ) * 16 - 16)/2 +  (( (MAP_X + MAP_WIDTH ) * 16 - 16)/2)/5 ){

			if(upOrDown == 1){   //up right
				x+=20;
				y-=40;
			}else{     //down right
				x+=20;
				y+=45;
			}
		}
	}

/*
	tl = &map[ ( y / 16 ) * MAP_WIDTH + ( x / 16 ) ];
	tc = &map[ ( y / 16 ) * MAP_WIDTH + ( ( x + 7 ) / 16 ) ];
	tr = &map[ ( y / 16 ) * MAP_WIDTH + ( ( x + 15 ) / 16 ) ];
	cl = &map[ ( ( y + 7 ) / 16 ) * MAP_WIDTH + ( x / 16 ) ];
	cc = &map[ ( ( y + 7 ) / 16 ) * MAP_WIDTH + ( ( x + 7 ) / 16 ) ];
	cr = &map[ ( ( y + 7 ) / 16 ) * MAP_WIDTH + ( ( x + 15 ) / 16 ) ];
	bl = &map[ ( ( y + 15 ) / 16 ) * MAP_WIDTH + ( x / 16 ) ];
	bc = &map[ ( ( y + 15 ) / 16 ) * MAP_WIDTH + ( ( x + 7 ) / 16 ) ];
	br = &map[ ( ( y + 15 ) / 16 ) * MAP_WIDTH + ( ( x + 15 ) / 16 ) ];

	if( tank->x != x || tank->y != y ) {
		// Tank can move if water, iron or brick wall isn't ahead.
		if( tl->ptr != IMG_16x16_plavacigla && tl->ptr != IMG_16x16_cigle && tl->ptr != IMG_16x16_enemi1 &&
			tc->ptr != IMG_16x16_plavacigla && tc->ptr != IMG_16x16_cigle && tc->ptr != IMG_16x16_enemi1 &&
			tr->ptr != IMG_16x16_plavacigla && tr->ptr != IMG_16x16_cigle && tr->ptr != IMG_16x16_enemi1 &&
			cl->ptr != IMG_16x16_plavacigla && cl->ptr != IMG_16x16_cigle && cl->ptr != IMG_16x16_enemi1 &&
			cc->ptr != IMG_16x16_plavacigla && cc->ptr != IMG_16x16_cigle && cc->ptr != IMG_16x16_enemi1 &&
			cr->ptr != IMG_16x16_plavacigla && cr->ptr != IMG_16x16_cigle && cr->ptr != IMG_16x16_enemi1 &&
			bl->ptr != IMG_16x16_plavacigla && bl->ptr != IMG_16x16_cigle && bl->ptr != IMG_16x16_enemi1 &&
			bc->ptr != IMG_16x16_plavacigla && bc->ptr != IMG_16x16_cigle && bc->ptr != IMG_16x16_enemi1 &&
			br->ptr != IMG_16x16_plavacigla && br->ptr != IMG_16x16_cigle && br->ptr != IMG_16x16_enemi1  ) {

			tank->x = x;
			tank->y = y;

			if( tank->dir != dir ) {
				tank->dir = dir;

				Xil_Out32( XPAR_BATTLE_CITY_PERIPH_0_BASEADDR + 4 * ( REGS_BASE_ADDRESS + tank->reg_l ), (unsigned int)0x8F000000  | tank->type );
			}

			Xil_Out32( XPAR_BATTLE_CITY_PERIPH_0_BASEADDR + 4 * ( REGS_BASE_ADDRESS + tank->reg_h ), ( tank->y << 16 ) | tank->x );

			return b_true;
		}
	}*/
}
static void coin_destroy( characters * coin,unsigned int x, unsigned int y){
	coin->destroyed = b_true;

	Xil_Out32( XPAR_BATTLE_CITY_PERIPH_0_BASEADDR + 4 * ( REGS_BASE_ADDRESS + coin->reg_l ), (unsigned int)0x8F000000  | IMG_16x16_crno );
	Xil_Out32( XPAR_BATTLE_CITY_PERIPH_0_BASEADDR + 4 * ( REGS_BASE_ADDRESS + coin->reg_h ), ( coin>y << 16 ) | coin->x );

}
static bool_t mario_move( unsigned char ** map, characters * mario, direction_t dir)
{
		unsigned int    x;
	    unsigned int    y;


	    unsigned char *   mario_left;
	    unsigned char *   mario_right;
	    unsigned char *   mario_center;
	    unsigned char *   coin_left;
	    unsigned char *   coin_right;
	    unsigned char *   coin_center;
	    unsigned char *   brick_left;
	    unsigned char *   brick_right;
	    unsigned char *   brick_center;



	    if( mario->x > (( MAP_X + MAP_WIDTH ) * 16 - 16 ) ||
	    	mario->y > ( MAP_Y + MAP_HEIGHT ) * 16 - 16 ) {
	        return b_false;
	    }

	    x = mario->x;
	    y = mario->y;

	    // Make sure that coordinates will stay within map boundaries after moving.
	    if( dir == DIR_LEFT ) {
	        if( x > MAP_X * 16 )
	        	x--;
	    } else if( dir == DIR_RIGHT ) {
	        if( x < ( MAP_X + MAP_WIDTH ) * 16 - 16 )
	        	x++;
	    } else if( dir == DIR_UP ) {
	        if( y > MAP_Y * 16 )
	            y--;
	    } else if( dir == DIR_DOWN ) {
	        if( y < ( MAP_X + MAP_WIDTH ) * 16 - 16)
	            y++;
	    }

	    //static coins
	    /*if(mario->x == 454 && mario->y == 304 ){
	    	if(prvi1 = 1){
	    		coin_destroy(&coin1,x,y);
	    		map_update( map1 );
	    		prvi1 = 0;
	    	}
	    }else if (mario->x == 469 && mario->y == 304 ){
	    	if(prvi2 == 1){
	    		coin_destroy(&coin2,x,y);
	    		map_update( map1 );
	    		prvi2 = 0;
	    	}

	    }else if(mario->x == 484 && mario->y == 304 ){
	    	if(prvi3 = 1){
	    		coin_destroy(&coin3,x,y);
	    		map_update( map1 );
	    		prvi3 = 0;
	    	}

	    }*/




	    /*mario_left = &map[ ( y / 16 ) * MAP_WIDTH + ( x / 16 ) ];
	    mario_center = &map[ ( y / 16 ) * MAP_WIDTH + ( ( x + 7 ) / 16 ) ];
	    mario_right = &map[ ( y / 16 ) * MAP_WIDTH + ( ( x + 15 ) / 16 ) ];
		/*coin_left = &map[ ( ( y + 7 ) / 16 ) * MAP_WIDTH + ( x / 16 ) ];
		coin_center = &map[ ( ( y + 7 ) / 16 ) * MAP_WIDTH + ( ( x + 7 ) / 16 ) ];
		coin_right = &map[ ( ( y + 7 ) / 16 ) * MAP_WIDTH + ( ( x + 15 ) / 16 ) ];
		brick_left = &map[ ( ( y + 15 ) / 16 ) * MAP_WIDTH + ( x / 16 ) ];
		brick_center = &map[ ( ( y + 15 ) / 16 ) * MAP_WIDTH + ( ( x + 7 ) / 16 ) ];
		brick_right = &map[ ( ( y + 15 ) / 16 ) * MAP_WIDTH + ( ( x + 15 ) / 16 ) ];*/

	    /*if( mario->x != x || mario->y != y ) {
	        // Tank can move if water, iron or brick wall isn't ahead.
	        if( tl->ptr != IMG_16x16_plavacigla && tl->ptr != IMG_16x16_cigle && tl->ptr != IMG_16x16_enemi1 &&
				tc->ptr != IMG_16x16_plavacigla && tc->ptr != IMG_16x16_cigle && tc->ptr != IMG_16x16_enemi1 &&
				tr->ptr != IMG_16x16_plavacigla && tr->ptr != IMG_16x16_cigle && tr->ptr != IMG_16x16_enemi1 &&
				cl->ptr != IMG_16x16_plavacigla && cl->ptr != IMG_16x16_cigle && cl->ptr != IMG_16x16_enemi1 &&
				cc->ptr != IMG_16x16_plavacigla && cc->ptr != IMG_16x16_cigle && cc->ptr != IMG_16x16_enemi1 &&
				cr->ptr != IMG_16x16_plavacigla && cr->ptr != IMG_16x16_cigle && cr->ptr != IMG_16x16_enemi1 &&
	        	bl->ptr != IMG_16x16_plavacigla && bl->ptr != IMG_16x16_cigle && bl->ptr != IMG_16x16_enemi1 &&
	        	bc->ptr != IMG_16x16_plavacigla && bc->ptr != IMG_16x16_cigle && bc->ptr != IMG_16x16_enemi1 &&
	        	br->ptr != IMG_16x16_plavacigla && br->ptr != IMG_16x16_cigle && br->ptr != IMG_16x16_enemi1 ) {

	            tank->x = x;
	            tank->y = y;

	            if( tank->dir != dir ) {
	                tank->dir = dir;

	                Xil_Out32( XPAR_BATTLE_CITY_PERIPH_0_BASEADDR + 4 * ( REGS_BASE_ADDRESS + tank->reg_l ), (unsigned int)0x8F000000  | tank->type );
	            }

	            Xil_Out32( XPAR_BATTLE_CITY_PERIPH_0_BASEADDR + 4 * ( REGS_BASE_ADDRESS + tank->reg_h ), ( tank->y << 16 ) | tank->x );

	            return b_true;

	        } /*else if( bc->ptr == IMG_16x16_crno){

	        	 tank->x = x;
	        	 tank->y = y+65;

	        	 if( tank->dir != dir ) {
	        		 tank->dir = dir;

	        		 Xil_Out32( XPAR_BATTLE_CITY_PERIPH_0_BASEADDR + 4 * ( REGS_BASE_ADDRESS + tank->reg_l ), (unsigned int)0x8F000000  | tank->type );
	        	}

	        	 	 Xil_Out32( XPAR_BATTLE_CITY_PERIPH_0_BASEADDR + 4 * ( REGS_BASE_ADDRESS + tank->reg_h ), ( tank->y << 16 ) | tank->x );

	        		 return b_true;

	        }


	    }*/

	return b_false;
}

static void process_ai( characters * tank, unsigned int * ai_dir )
{
	unsigned int	tmp_dir;
	bool_t			turn;
	unsigned int	i;


	if( turn == b_true ) {
		do {
			while( tmp_dir == *ai_dir ) {
				tmp_dir = rand_lfsr113( ) % 2;
			}

			*ai_dir = tmp_dir;

		} while( mario_move( map1, &enemie, *ai_dir) == b_false );
	} else {
		while( mario_move( map1, &enemie, *ai_dir ) == b_false ) {
			while( tmp_dir == *ai_dir ) {
				tmp_dir = rand_lfsr113( ) % 2;
			}

			*ai_dir = tmp_dir;
		}
	}

	i = 0;


	if( *ai_dir == DIR_RIGHT ) {
		while( tank->x / 8 + i < (MAP_X + MAP_WIDTH)/2 ) {
			if( map1[ ( tank->y / 8 + i ) * MAP_WIDTH + tank->x / 12 ] == IMG_16x16_enemi1)
				{
					break;
				}
			i++;
		}

	} else if( *ai_dir == DIR_LEFT ) {
		while( tank->x / 8 + i < (MAP_X + MAP_WIDTH)/2 ) {
			if( map1[ ( tank->y / 8 + i ) * MAP_WIDTH + tank->x / 8 ] == IMG_16x16_enemi1)
			{
		      break;
		    }
		i++;

		}
	}
}


void battle_city( void )
{

	unsigned int buttons, tmpBtn = 0, tmpUp = 0;
	int i,change = 0;

	map_reset( map1 );
	map_update();



/*
	//chhar_spawn(&coin1);
	//chhar_spawn(&coin2);
	//chhar_spawn(&coin3);
	chhar_spawn(&enemie);
	chhar_spawn(&mario);

	while( 1 ) {

    	   buttons = XIo_In32( XPAR_IO_PERIPH_BASEADDR );

			if( BTN_LEFT( buttons ) ) {
				mario_move( map1, &mario, DIR_LEFT );
				tmpBtn = 1;
				tmpUp = 0;
			} else if( BTN_RIGHT( buttons ) ) {
				mario_move( map1, &mario, DIR_RIGHT );
				tmpBtn = 0;
				tmpUp = 0;
			} else if( BTN_UP( buttons ) ) {

				if(tmpBtn == 1 ){
									jump( map1, &mario, DIR_LEFT, 1 ); //up
									for(i = 0; i < 1000000; i++){}
									jump( map1, &mario, DIR_LEFT, 0 ); //down
								}
				else if(tmpBtn == 0 )
								{
									jump( map1, &mario, DIR_RIGHT, 1 );  //up
									for(i = 0; i < 1000000; i++){}
									jump( map1, &mario, DIR_RIGHT, 0 );  //down
									mario_move( map1, &mario, DIR_DOWN );
									mario_move( map1, &mario, DIR_DOWN );
									mario_move( map1, &mario, DIR_DOWN );
								}
								tmpUp = 1;
			} else if( BTN_DOWN( buttons ) ) {
				mario_move( map1, &mario, DIR_DOWN );
				tmpUp = 0;
			}

		process_ai( &enemie, DIR_LEFT );
		for(i = 0; i < 300000; i++){}

        map_update( map1 );

    }*/
}
