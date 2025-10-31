#include <bits/stdc++.h>
using namespace std;

const double EPS = 1e-9;
const double LIMIT_ABS = 1e12;

string bestSolution;
bool hasBest = false;

bool isInteger(double x) {
    return fabs(x - round(x)) < 1e-9;
}

double safePow(double a, double b, bool &ok) {
    ok = false;
    if (!isInteger(b)) return 0;
    long long ib = llround(b);
    if (ib < 0 || ib > 12) return 0;
    if (a == 0 && ib == 0) return 0;
    double res = pow(a, (double)ib);
    if (!isfinite(res) || fabs(res) > LIMIT_ABS) return 0;
    ok = true;
    return res;
}

void tryMergeAndRecur(const vector<double>& vals,
                      const vector<string>& exprs,
                      double target,
                      set<string>& solutions) {
    int n = vals.size();
    if (n == 1) {
        if (fabs(vals[0] - target) < 1e-8) {
            string out = exprs[0];
            if (out.size() >= 2 && out.front() == '(' && out.back() == ')') {
                int depth = 0; bool canStrip = true;
                for (size_t i = 0; i < out.size(); ++i) {
                    if (out[i] == '(') depth++;
                    else if (out[i] == ')') depth--;
                    if (depth == 0 && i + 1 < out.size()) { canStrip = false; break; }
                }
                if (canStrip) out = out.substr(1, out.size() - 2);
            }
            long long tgtInt = llround(target);
            ostringstream oss;
            oss << out << "=" << tgtInt;
            string expr = oss.str();
            solutions.insert(expr);
            if (!hasBest || expr.size() < bestSolution.size() ||
                (expr.size() == bestSolution.size() && expr < bestSolution)) {
                bestSolution = expr;
                hasBest = true;
            }
        }
        return;
    }

    for (int i = 0; i < n - 1; ++i) {
        for (int op = 0; op < 5; ++op) {
            double a = vals[i], b = vals[i+1];
            string sa = exprs[i], sb = exprs[i+1];
            double res = 0;
            bool ok = true;
            string opstr;
            if (op == 0) {
                res = a + b; opstr = "+";
            } else if (op == 1) {
                res = a - b; opstr = "-";
            } else if (op == 2) {
                res = a * b; opstr = "*";
            } else if (op == 3) {
                if (fabs(b) < EPS) ok = false;
                else res = a / b;
                opstr = "/";
            } else {
                bool okPow;
                res = safePow(a, b, okPow);
                ok = okPow;
                opstr = "^";
            }
            if (!ok) continue;
            if (!isfinite(res) || fabs(res) > LIMIT_ABS) continue;

            vector<double> nvals;
            vector<string> nexprs;
            for (int j = 0; j < i; ++j) {
                nvals.push_back(vals[j]);
                nexprs.push_back(exprs[j]);
            }
            string merged = "(" + sa + opstr + sb + ")";
            nvals.push_back(res);
            nexprs.push_back(merged);
            for (int j = i + 2; j < n; ++j) {
                nvals.push_back(vals[j]);
                nexprs.push_back(exprs[j]);
            }

            tryMergeAndRecur(nvals, nexprs, target, solutions);
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout << "Input a number (5-9 digits): " << flush;
    string s;
    if (!(cin >> s)) return 0;
    if (s.size() < 5 || s.size() > 9 || !all_of(s.begin(), s.end(), ::isdigit)) {
        cout << "Invalid input. Must be 5 to 9 digits.\n";
        return 0;
    }

    set<string> allSolutions;
    bestSolution.clear();
    hasBest = false;

    for (int split = 1; split < (int)s.size(); ++split) {
        string left = s.substr(0, split);
        string right = s.substr(split);
        long long target = stoll(right);
        int m = left.size();
        int ways = 1 << (m - 1);
        for (int mask = 0; mask < ways; ++mask) {
            vector<double> vals;
            vector<string> exprs;
            string cur;
            for (int i = 0; i < m; ++i) {
                cur.push_back(left[i]);
                if (i == m - 1 || (mask & (1 << i))) {
                    double v = stod(cur);
                    vals.push_back(v);
                    exprs.push_back(cur);
                    cur.clear();
                }
            }
            set<string> solutions;
            tryMergeAndRecur(vals, exprs, (double)target, solutions);
            allSolutions.insert(solutions.begin(), solutions.end());
        }
    }

    if (allSolutions.empty()) {
        cout << "No solution found.\n";
    } else {
        cout << "Solutions found:\n";
        for (const auto& str : allSolutions) {
            cout << str << "\n";
        }
        cout << "Shortest solution:\n" << bestSolution << "\n";
    }
    cout << "Press Enter to exit..." << flush;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
    return 0;
}
