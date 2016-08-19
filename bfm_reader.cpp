#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mat.h"

#include <fstream>
#include <iostream>
#include <functional>

using namespace std;


void fetch_tri_list(mxArray *pa, const char *outfile)
{
    ios_base::sync_with_stdio(false);
    ofstream ofs(outfile);

    if (ofs.good()) {

        size_t ntl = mxGetM(pa);
        double *data = static_cast<double*>(mxGetData(pa));

        ofs << "# " << ntl << " x 3\n";
        for (int i=0; i<ntl; ++i) {
            ofs << static_cast<int>(data[i]) << ' '
                << static_cast<int>(data[i+ntl]) << ' '
                << static_cast<int>(data[i+ntl+ntl])
                << '\n';
        }

        ofs.close();
        mxDestroyArray(pa);
        cout << "Fetching done: " << outfile << endl;
    }
    else {
        mxDestroyArray(pa);
        cout << "Error on opening " << outfile << " for writing" << endl;
    }

}


void write_array(ofstream& ofs, float *data, size_t nb_rows, size_t nb_cols)
{
    ofs << "# " << nb_cols << " x " << nb_rows << "\n";

    for (size_t col = 0; col < nb_cols; col += 1) {

        for (size_t row = 0; row < nb_rows; row += 1) {
            ofs << data[row]   << ' ';
        }
        ofs << '\n';
    }

    ofs.close();
}

void fetch_data(mxArray *pa, const char *outfile)
{
    ios_base::sync_with_stdio(false);
    ofstream ofs(outfile);

    if (ofs.good()) {

        size_t nb_rows = mxGetM(pa);
        size_t nb_cols = mxGetN(pa);
        float *data = static_cast<float*>(mxGetData(pa));

        write_array(ofs, data, nb_rows, nb_cols);

        mxDestroyArray(pa);

        cout << "Fetching done: " << outfile << endl;
    }
    else {
        mxDestroyArray(pa);
        cout << "Error on opening " << outfile << " for writing" << endl;
    }
}

int fetch(const char *file) {

    printf("Reading file %s...\n\n", file);
    MATFile *pmat = matOpen(file, "r");
    if (pmat == NULL) {
        printf("Error cannot open file %s\n", file);
        return 1;
    }
    else {
        string filename(file);
        size_t last_slash_idx = filename.find_last_of("/\\");
        string path = filename.substr(0, last_slash_idx);

        auto fetch = [pmat, &path] (char* var_name, function<void(mxArray *, const char *)> fetch_func) {
            mxArray *pa = matGetVariable(pmat, var_name);
            if (pa == nullptr) {
                printf("Error getting variable %s\n", var_name);
            }
            else {
                string outfile = path + "/" + var_name + ".txt";
                fetch_func(pa, outfile.c_str());
            }
        };

        fetch("tl",      fetch_tri_list);
        fetch("shapeMU", fetch_data);
        fetch("texMU",   fetch_data);
        fetch("shapePC", fetch_data);
        fetch("texPC",   fetch_data);
        fetch("shapeEV", fetch_data);
        fetch("texEV", fetch_data);

        if (matClose(pmat) != 0) {
            printf("Error closing file %s\n", file);
            return 1;
        }

        printf("finish reading content\n");
        return 0;
    }

}

int main(int argc, char **argv)
{
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " bdm_model_path" << endl;
        return 0;
    }
    else {
        int result = fetch(argv[1]);
        return (result == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
    }
}
