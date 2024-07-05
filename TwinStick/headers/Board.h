#pragma once

class Board {
private:
	sg::Object3D* pieceObjs[16];
    int board[4][4];

public:
    sg::Object3D* boardObj;

    void SwapPieces(int p1x, int p1y, int p2x, int p2y) {
        if (p1x < 0 || p1y < 0 || p2x < 0 || p2y < 0 || p1x >= 4 || p1y >= 4 || p2x >= 4 || p2y >= 4) return;
        sg::Object3D* piece1 = pieceObjs[board[p1x][p1y]];
        sg::Object3D* piece2 = pieceObjs[board[p2x][p2y]];
        glm::vec3 pos1 = piece1->GetLocalPosition();
        piece1->SetLocalPosition(piece2->GetLocalPosition());
        piece2->SetLocalPosition(pos1);
        int ind1 = board[p1x][p1y];
        board[p1x][p1y] = board[p2x][p2y];
        board[p2x][p2y] = ind1;
    }

    void PositionPieces(int pieces[16]) {
        for (int i = 0; i < 16; i++) {
            pieceObjs[pieces[i]]->SetLocalPosition(-18.75 + 12.5 * (i / 4), 0.01, -18.75 + 12.5 * (i % 4));
            board[i / 4][i % 4] = pieces[i];
        }
    }

    void Init(sg::Renderer *renderer) {
        boardObj = new sg::Object3D();
        boardObj->LoadModelFromObj("res/models/board.obj");
        boardObj->SetLocalScale(50, 50, 50);
        boardObj->CastsShadows = true;
        boardObj->ReceivesShadows = true;
        boardObj->Lit = true;

        for (int i = 0; i < 16; i++) {
            pieceObjs[i] = new sg::Object3D();
            pieceObjs[i]->SetLocalPosition(-18.75 + 12.5 * (i / 4), 0.01, -18.75 + 12.5 * (i % 4));
            pieceObjs[i]->SetLocalScale(50, 50, 50);
            pieceObjs[i]->CastsShadows = true;
            pieceObjs[i]->ReceivesShadows = true;
            pieceObjs[i]->Lit = true;
            board[i / 4][i % 4] = i;
        }

        pieceObjs[0]->LoadModelFromObj("res/models/piece_0000.obj");
        pieceObjs[1]->LoadModelFromObj("res/models/piece_0001.obj");
        pieceObjs[2]->LoadModelFromObj("res/models/piece_0010.obj");
        pieceObjs[3]->LoadModelFromObj("res/models/piece_0011.obj");
        pieceObjs[4]->LoadModelFromObj("res/models/piece_1100.obj");
        pieceObjs[5]->LoadModelFromObj("res/models/piece_1101.obj");
        pieceObjs[6]->LoadModelFromObj("res/models/piece_1110.obj");
        pieceObjs[7]->LoadModelFromObj("res/models/piece_1111.obj");
        for (int i = 8; i < 16; i++) {
            pieceObjs[i]->SetModel(pieceObjs[i - 8]->GetModel());
        }

        sg::Material white = pieceObjs[0]->GetMaterialAt(0);
        sg::Material black = pieceObjs[4]->GetMaterialAt(0);
        for (int i = 4; i < 8; i++) {
            pieceObjs[i]->ChangeMaterial(black.name, white);
        }
        for (int i = 8; i < 12; i++) {
            pieceObjs[i]->ChangeMaterial(white.name, black);
        }

        int newOrder[16] = { 1, 0, 2, 3, 4, 5, 7, 6, 12, 13, 15, 14, 9, 8, 10, 11 };
        PositionPieces(newOrder);

        renderer->AddObject(boardObj);
        for (int i = 0; i < 16; i++)
            renderer->AddObject(pieceObjs[i]);
    }

    ~Board() {
        for (int i = 0; i < 16; i++) {
            free(pieceObjs[i]);
        }
        free(boardObj);
    }

};