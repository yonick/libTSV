#ifndef UTILITIES_H_INCLUDED
#define UTILITIES_H_INCLUDED

class noCopy {
protected:
    noCopy()  {}
    ~noCopy() {}
private:
    noCopy(const noCopy&);
    noCopy& operator=(const noCopy&);
};


#endif // UTILITIES_H_INCLUDED