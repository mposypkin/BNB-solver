/* 
 * Author: posypkin
 *
 * Created on April 24, 2011, 5:52 AM
 */

#include <map>
#include <util/poly/polyutil.hpp>
#include <problems/optlib/polyobjective.hpp>
#include <problems/optlib/gradboxdesc.hpp>
#include <util/ineq/foumotz.hpp>

#include "mboxlocsearch.hpp"
#include "mgradesc.hpp"

template <typename FT> class Karp1 {
public:

    /**
     * Fills in source data for the problem
     * @return the filled data
     */
    static MultiData<FT> fillInData() {

        std::map<std::string, int> idents;
        idents["x1"] = 0;
        idents["x2"] = 1;
        Polynom<double>* poly1 = new Polynom<double>();
        PolyUtil::fromString("x1^2 + x2^2 - 10 x1 + 35", idents, *poly1);
        PolyObjective<double>* pobj1 = new PolyObjective<double>(poly1);
        Polynom<double>* poly2 = new Polynom<double>();
        PolyUtil::fromString("x1^2 + x2^2 - 10 x2 + 45", idents, *poly2);
        PolyObjective<double>* pobj2 = new PolyObjective<double>(poly2);
        Box<double> box(2);
        box.mA[0] = 0;
        box.mA[1] = 0;
        box.mB[0] = 5;
        box.mB[1] = 5;
        MultiData<double> md(box);
        md.mCriteria.push_back((Objective<double>*) pobj1);
        md.mCriteria.push_back((Objective<double>*) pobj2);
        return md;
    }

};


class MyStopper : public GradBoxDescent<double>::Stopper {
    public:
        bool stopnow(double xdiff, double fdiff, double* x, double* grad, double fval, int n) {
            if(xdiff <= 0.001) {
                return true;
            }
        }
    };
 
    
void initialize(const MultiData<double>& md, MultiPoint<double>& mp) {
    /*
    mp.mParam.mX[0] = 3.43694;
    mp.mParam.mX[1] = 0.4982;
     */
    mp.mParam.mX[0] = 16.3741;
    mp.mParam.mX[1] = 40.6706;
     
    mp.mCrit.mX[0] = md.mCriteria[0]->func(mp.mParam.mX);
    mp.mCrit.mX[1] = md.mCriteria[1]->func(mp.mParam.mX);
}

int main(int argc, char** argv) {
    MultiData<double> md = Karp1<double>::fillInData();
    MyStopper stopper;
    GradBoxDescent<double> gbd(md.mBox, &stopper);
    std::vector<double> coe;
    coe.push_back(1.);
    coe.push_back(1.);
    MBoxLocalSearch<double> mbls(md, coe, &gbd);
    MultiPoint<double> mp(2, 2);
    initialize(md, mp);
    printf("source point: %s\n", mp.toString().c_str());
    mbls.search(mp);
    printf("resulting point after linear convolution: %s\n", mp.toString().c_str());

    FourierMotzkin<double> fm;
    MultiGradDescent<double> mgd(md, &fm);
    initialize(md, mp);
    mgd.search(mp);
    printf("resulting point after multiobjective descent search: %s\n", mp.toString().c_str());
}

