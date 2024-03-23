#pragma once
#include <sgObject3D.h>
#include <sgRenderer.h>

class Board {
private:
	sg::Object3D pieceObjs[16];
    sg::Object3D boardObj;
    int board[4][4];

public:
    void Init(sg::Renderer *renderer) {
        boardObj.LoadModelFromObj("res/models/board.obj");
        boardObj.SetLocalScale(50, 50, 50);
        boardObj.CastsShadows = true;
        boardObj.ReceivesShadows = true;
        boardObj.Lit = true;

        for (int i = 0; i < 16; i++) {
            pieceObjs[i] = sg::Object3D();
            pieceObjs[i].SetLocalPosition(-18.75 + 12.5 * (i / 4), 0.01, -18.75 + 12.5 * (i % 4));
            pieceObjs[i].SetLocalScale(50, 50, 50);
            pieceObjs[i].CastsShadows = true;
            pieceObjs[i].ReceivesShadows = true;
            pieceObjs[i].Lit = true;
            board[i / 4][i % 4] = i;
        }

        pieceObjs[0].LoadModelFromObj("res/models/piece_0000.obj");
        pieceObjs[1].LoadModelFromObj("res/models/piece_0001.obj");
        pieceObjs[2].LoadModelFromObj("res/models/piece_0010.obj");
        pieceObjs[3].LoadModelFromObj("res/models/piece_0011.obj");
        pieceObjs[4].LoadModelFromObj("res/models/piece_1100.obj");
        pieceObjs[5].LoadModelFromObj("res/models/piece_1101.obj");
        pieceObjs[6].LoadModelFromObj("res/models/piece_1110.obj");
        pieceObjs[7].LoadModelFromObj("res/models/piece_1111.obj");
        for (int i = 8; i < 16; i++) {
            pieceObjs[i].SetModel(pieceObjs[i - 8].GetModel());
        }

        renderer->AddObject(&boardObj);
        for (int i = 0; i < 16; i++)
            renderer->AddObject(&pieceObjs[i]);
    }

};