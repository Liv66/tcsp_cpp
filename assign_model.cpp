#include "assign_model.h"
#include "gurobi_c++.h"
#include <iostream>
using namespace std;

int run_mip() {
    try {
        GRBEnv env = GRBEnv(true);
        env.start();

        GRBModel model = GRBModel(env);

        // 변수 생성: x, y ≥ 0
        GRBVar x = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "x");
        GRBVar y = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "y");

        // 목적함수: maximize 3x + 4y
        model.setObjective(3*x + 4*y, GRB_MAXIMIZE);

        // 제약식: x + 2y ≤ 14
        model.addConstr(x + 2*y <= 14, "c0");

        // 제약식: 3x - y ≥ 0
        model.addConstr(3*x - y >= 0, "c1");

        // 제약식: x - y ≤ 2
        model.addConstr(x - y <= 2, "c2");

        // 최적화 수행
        model.optimize();

        // 결과 출력
        cout << "x = " << x.get(GRB_DoubleAttr_X) << endl;
        cout << "y = " << y.get(GRB_DoubleAttr_X) << endl;
        cout << "Obj = " << model.get(GRB_DoubleAttr_ObjVal) << endl;

    } catch (GRBException e) {
        cout << "Gurobi error: " << e.getMessage() << endl;
    } catch (...) {
        cout << "Unknown error" << endl;
    }

    return 0;
}
