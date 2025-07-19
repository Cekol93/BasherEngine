#include "engineRendererDrawBuffer.h"


engineRenderDrawBuffer::engineRenderDrawBuffer(int a) {

    maxQuads = a;
    frontBufferV = new float[8 * maxQuads]; // 4(quad)x2(point)x2(texture&verts) FOR MULTITHREADING
    frontBufferT = new float[8 * maxQuads];
    frontBufferC = new float[16 * maxQuads];

    backBufferV = new float[8 * maxQuads];
    backBufferT = new float[8 * maxQuads];
    backBufferC = new float[16 * maxQuads];

    cR = 1.0f;
    cG = 1.0f;
    cB = 1.0f;
    cA = 1.0f;

    editedBufferV = frontBufferV;
    editedBufferT = frontBufferT;
    editedBufferC = frontBufferC;

    drawedBufferV = backBufferV;
    drawedBufferT = backBufferT;
    drawedBufferC = backBufferC;

    editedQuads = 0;
    drawedQuads = 0;

    //cout<<"Draw buffer created"<<endl;
}

engineRenderDrawBuffer::~engineRenderDrawBuffer() {
    delete[] frontBufferV;
    delete[] frontBufferT;
    delete[] backBufferV;
    delete[] backBufferT;
}

void engineRenderDrawBuffer::setAlpha(float _a) { cA = _a; }
void engineRenderDrawBuffer::setColor(float _r, float _g, float _b) { cR = _r; cG = _g; cB = _b; }

/*
void draw_buffer::draw_quad(int Fr , Vec2 a,Vec2 b,Vec2 c,Vec2 d , int Col,int Row ){

    int frame =  max(0,min(Fr,Col*Row));
    int Tfr = frame*8;
    int Tptd = editedQuads*8;

    float Tx = 1.00000f/(float)Col;
    float Ty = 1.00000f/(float)Row;
    float xx = (frame%Col);
    float yy = (floor(frame/Col));

    draw_quad_generic( 0
                      , a
                      , b
                      , c
                      , d
                      , Vec2( 0+(Tx*xx) , 0+(Ty*yy) )
                      , Vec2( Tx+(Tx*xx) , 0+(Ty*yy) )
                      , Vec2( Tx+(Tx*xx) , Ty+(Ty*yy) )
                      , Vec2( 0+(Tx*xx) , Ty+(Ty*yy ) )
    );

}*/
/*
void draw_buffer::draw_quad_generic( int li , Vec2 a , Vec2 b , Vec2 c , Vec2 d , Vec2 ta, Vec2 tb , Vec2 tc , Vec2 td ){


    if (li == 0){
        if (editedQuads<maxQuads){

            int Tptd = editedQuads*8;
            editedBufferT[Tptd + 0] = ta.x;   //Texturing
            editedBufferT[Tptd + 1] = ta.y;
            editedBufferT[Tptd + 2] = tb.x;
            editedBufferT[Tptd + 3] = tb.y;
            editedBufferT[Tptd + 4] = tc.x;
            editedBufferT[Tptd + 5] = tc.y;
            editedBufferT[Tptd + 6] = td.x;
            editedBufferT[Tptd + 7] = td.y; // End of texturing

            editedBufferV[Tptd +0] = a.x;
            editedBufferV[Tptd +1] = a.y;
            editedBufferV[Tptd +2] = b.x;
            editedBufferV[Tptd +3] = b.y;
            editedBufferV[Tptd +4] = c.x;
            editedBufferV[Tptd +5] = c.y;
            editedBufferV[Tptd +6] = d.x;
            editedBufferV[Tptd +7] = d.y;

            int cI = editedQuads*16;
            for (int i = 0 ; i<4 ; i++){
                editedBufferC[cI +0 +i*4] = c_r; //r
                editedBufferC[cI +1 +i*4] = c_g; //g
                editedBufferC[cI +2 +i*4] = c_b; //b
                editedBufferC[cI +3 +i*4] = c_a; //a
            }
            editedQuads+=1;
        }
    }
    else{
        Vec2 center,tcenter;
        Vec2 ab,ad,bc,cd; // Half positions of verts
        Vec2 tab,tad,tbc,tcd;

        ab = a+b; ab*=0.5f;
        ad = a+d; ad*=0.5f;
        bc = b+c; bc*=0.5f;
        cd = c+d; cd*=0.5f;

        tab = ta+tb; tab*=0.5f;
        tad = ta+td; tad*=0.5f;
        tbc = tb+tc; tbc*=0.5f;
        tcd = tc+td; tcd*=0.5f;

        center = a+b+c+d; center*=0.25f;
        tcenter = ta+tb+tc+td; tcenter*=0.25f;

        draw_quad_generic( li-1 , a , ab ,  center ,  ad ,  ta , tab ,  tcenter ,  tad ); //1
        draw_quad_generic( li-1 , ab ,  b , bc , center , tab ,  tb , tbc , tcenter ); //2
        draw_quad_generic( li-1 , ad , center , cd , d , tad , tcenter , tcd , td ); //3
        draw_quad_generic( li-1 , center , bc , c , cd , tcenter , tbc , tc , tcd ); //4

    }


    //cout<<frame<<" / "<<Tx<<"/"<<Ty<<"/ "<<xx<<"/"<<yy<<"/["<<(frame%Col)<<"/"<<floor(frame/Col)<<"]"<<endl;


}*/


void engineRenderDrawBuffer::drawQuadGeneric(Vec2 a, Vec2 b, Vec2 c, Vec2 d, Vec2 ta, Vec2 tb, Vec2 tc, Vec2 td) {
    if (editedQuads == maxQuads) return;

    int Tptd = editedQuads * 8;
    editedBufferT[Tptd + 0] = ta.x;   //Texturing
    editedBufferT[Tptd + 1] = ta.y;
    editedBufferT[Tptd + 2] = tb.x;
    editedBufferT[Tptd + 3] = tb.y;
    editedBufferT[Tptd + 4] = tc.x;
    editedBufferT[Tptd + 5] = tc.y;
    editedBufferT[Tptd + 6] = td.x;
    editedBufferT[Tptd + 7] = td.y; // End of texturing


    editedBufferV[Tptd + 0] = a.x;
    editedBufferV[Tptd + 1] = a.y;
    editedBufferV[Tptd + 2] = b.x;
    editedBufferV[Tptd + 3] = b.y;
    editedBufferV[Tptd + 4] = c.x;
    editedBufferV[Tptd + 5] = c.y;
    editedBufferV[Tptd + 6] = d.x;
    editedBufferV[Tptd + 7] = d.y;

    int cI = editedQuads * 16;
    for (int i = 0; i < 4; i++) {
        editedBufferC[cI + 0 + i * 4] = cR; //r
        editedBufferC[cI + 1 + i * 4] = cG; //g
        editedBufferC[cI + 2 + i * 4] = cB; //b
        editedBufferC[cI + 3 + i * 4] = cA; //a
    }

    editedQuads += 1;
}


void engineRenderDrawBuffer::drawTile(int Fr, Vec2 Pos, Vec2 Size, float Ang, Vec2 Org, int Col, int Row) {

    if (editedQuads == maxQuads) return;

    float X = Pos.x;
    float Y = Pos.y;

    int frame = max(0, min(Fr, Col * Row));
    int Tfr = frame * 8;
    int Tptd = editedQuads * 8;

    float Tx = 1.00000f / (float)Col;
    float Ty = 1.00000f / (float)Row;
    float xx = (frame % Col);
    float yy = (floor(frame / Col));
    //cout<<frame<<" / "<<Tx<<"/"<<Ty<<"/ "<<xx<<"/"<<yy<<"/["<<(frame%Col)<<"/"<<floor(frame/Col)<<"]"<<endl;
    editedBufferT[Tptd + 0] = 0 + (Tx * xx);;   //Texturing
    editedBufferT[Tptd + 1] = 0 + (Ty * yy);
    editedBufferT[Tptd + 2] = Tx + (Tx * xx);
    editedBufferT[Tptd + 3] = 0 + (Ty * yy);
    editedBufferT[Tptd + 4] = Tx + (Tx * xx);
    editedBufferT[Tptd + 5] = Ty + (Ty * yy);
    editedBufferT[Tptd + 6] = 0 + (Tx * xx);
    editedBufferT[Tptd + 7] = Ty + (Ty * yy); // End of texturing

    Org.x *= Size.x;
    Org.y *= Size.y;
    editedBufferV[Tptd + 0] = (0 - Org.x) + Pos.x;
    editedBufferV[Tptd + 1] = (0 - Org.y) + Pos.y;
    editedBufferV[Tptd + 2] = (Size.x - Org.x) + Pos.x;
    editedBufferV[Tptd + 3] = (0 - Org.y) + Pos.y;
    editedBufferV[Tptd + 4] = (Size.x - Org.x) + Pos.x;
    editedBufferV[Tptd + 5] = (Size.y - Org.y) + Pos.y;
    editedBufferV[Tptd + 6] = (0 - Org.x) + Pos.x;
    editedBufferV[Tptd + 7] = (Size.y - Org.y) + Pos.y;

    int cI = editedQuads * 16;
    for (int i = 0; i < 4; i++) {
        editedBufferC[cI + 0 + i * 4] = cR; //r
        editedBufferC[cI + 1 + i * 4] = cG; //g
        editedBufferC[cI + 2 + i * 4] = cB; //b
        editedBufferC[cI + 3 + i * 4] = cA; //a
    }

    if (Ang != 0)
    {
        for (int a = 0; a < 8; a += 2)
        {

            float x, y, Angle, Distance;
            x = editedBufferV[Tptd + 0 + a] - X;
            y = editedBufferV[Tptd + 1 + a] - Y;
            Angle = (180 - (atan2(-y, -x) * angToReg) + Ang) * 0.01745329251f;
            Distance = sqrt(x * x + y * y);
            editedBufferV[Tptd + 0 + a] = (cos(Angle) * Distance) + X;
            editedBufferV[Tptd + 1 + a] = (-sin(Angle) * Distance) + Y;

        }
    }

    editedQuads += 1;

}

void engineRenderDrawBuffer::swapBuffers() {

    float* Temp;//,*TempT; // SWAP BUFFERS FOR MULTITHREADING
    //SWAPING VERTEX BUFFER
    Temp = editedBufferV;
    editedBufferV = drawedBufferV;
    drawedBufferV = Temp;
    //SWAPING TEXCORD BUFFER
    Temp = editedBufferT;
    editedBufferT = drawedBufferT;
    drawedBufferT = Temp;
    //SWAPPING COLOR BUFFER
    Temp = editedBufferC;
    editedBufferC = drawedBufferC;
    drawedBufferC = Temp;
    //SWAPING QUADS NUMBERS
    drawedQuads = editedQuads;
    editedQuads = 0;
};
void engineRenderDrawBuffer::render(GLuint texture) {
    //glPushMatrix();
    //glTranslated(M_getRender()->get_position_x(),M_getRender()->get_position_y(),0);
    //glColor4f(alpha,alpha,alpha,alpha);

    glBindTexture(GL_TEXTURE_2D, texture); // DRAWING MENU ON SCREEN...
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glVertexPointer(2, GL_FLOAT, 0, drawedBufferV);
    glTexCoordPointer(2, GL_FLOAT, 0, drawedBufferT);
    glColorPointer(4, GL_FLOAT, 0, drawedBufferC);

    glDrawArrays(GL_QUADS, 0, 4 * drawedQuads);

    //glColor4f(1.0f,1.0f,1.0f,1.0f);
    //glPopMatrix(); // END DRAWING SCENE
};

