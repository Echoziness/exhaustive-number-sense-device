#include <bits/stdc++.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <unordered_set>

using namespace std;

const double EPS = 1e-9;
const double LIMIT_ABS = 1e12;

string bestSolution;
bool hasBest = false;
mutex bestMutex;
mutex coutMutex;

struct TaskContext {
    chrono::steady_clock::time_point start_time;
    int timeout_ms;
    int check_counter;

    TaskContext(int timeout) : timeout_ms(timeout), check_counter(0) {
        start_time = chrono::steady_clock::now();
    }

    bool isTimeout() {
        check_counter++;
        if (check_counter >= 500) {
            check_counter = 0;
            auto now = chrono::steady_clock::now();
            return chrono::duration_cast<chrono::milliseconds>(now - start_time).count() > timeout_ms;
        }
        return false;
    }
};

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
                      unordered_set<string>& solutions,
                      TaskContext& ctx) {
    if (ctx.isTimeout()) return;

    if (hasBest) {
        size_t current_len = 0;
        for(const auto& s : exprs) current_len += s.size();
        if (current_len >= bestSolution.size()) return;
    }

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

            lock_guard<mutex> lock(bestMutex);
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

            tryMergeAndRecur(nvals, nexprs, target, solutions, ctx);
            if (ctx.isTimeout()) return;
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

    unordered_set<string> allSolutions;
    bestSolution.clear();
    hasBest = false;

    const int TIMEOUT_PER_TASK_MS = 10000;

    unsigned int numThreads = thread::hardware_concurrency();
    if (numThreads < 2) numThreads = 2;

    vector<thread> threads;
    vector<unordered_set<string>> threadSolutions(numThreads);

    cout << "Using " << numThreads << " threads.\n";

    int totalSplits = (int)s.size() - 1;

    for (int t = 0; t < (int)numThreads; ++t) {
        threads.emplace_back([&, t]() {
            for (int split = 1; split < (int)s.size(); ++split) {
                if ((split - 1) % numThreads != t) continue;

                string left = s.substr(0, split);
                string right = s.substr(split);

                {
                    lock_guard<mutex> lock(coutMutex);
                    cout << "Thread " << t << ": Trying " << left << " = " << right << "..." << endl;
                }

                long long target = stoll(right);
                int m = left.size();
                int ways = 1 << (m - 1);

                unordered_set<string> localSolutions;

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

                    TaskContext ctx(TIMEOUT_PER_TASK_MS);
                    tryMergeAndRecur(vals, exprs, (double)target, localSolutions, ctx);
                }

                if (!localSolutions.empty()) {
                    lock_guard<mutex> lock(coutMutex);
                    cout << "Thread " << t << ": Found " << localSolutions.size() << " solution(s) for " << left << endl;
                }

                threadSolutions[t].insert(localSolutions.begin(), localSolutions.end());
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    for (int t = 0; t < (int)numThreads; ++t) {
        allSolutions.insert(threadSolutions[t].begin(), threadSolutions[t].end());
    }

    if (allSolutions.empty()) {
        cout << "\nNo solution found.\n";
    } else {
        cout << "\nAll solutions found (" << allSolutions.size() << " total):\n";
        vector<string> sortedSolutions(allSolutions.begin(), allSolutions.end());
        sort(sortedSolutions.begin(), sortedSolutions.end(), [](const string& a, const string& b) {
            if (a.size() != b.size()) return a.size() < b.size();
            return a < b;
        });
        for (const auto& str : sortedSolutions) {
            cout << str << "\n";
        }
        cout << "\nShortest solution:\n" << bestSolution << "\n";
    }
    cout << "Press Enter to exit..." << flush;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
    return 0;
}
