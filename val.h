#ifndef VALUE_H
#define VALUE_H

#include <iostream>
#include <string>
#include <queue>
#include <map>
#include <iomanip>

using namespace std;

enum ValType { VINT, VREAL, VSTRING, VBOOL, VERR };

class Value {
    ValType	T;
    bool    Btemp;
	int		Itemp;
	float   Rtemp;
    string	Stemp;
       
public:
    Value() : T(VERR), Btemp(false), Itemp(0), Rtemp(0.0), Stemp("") {}
    Value(bool vb) : T(VBOOL), Btemp(vb), Itemp(0), Rtemp(0.0), Stemp("") {}
    Value(int vi) : T(VINT), Btemp(false), Itemp(vi), Rtemp(0.0), Stemp("") {}
	Value(float vr) : T(VREAL), Btemp(false), Itemp(0), Rtemp(vr), Stemp("") {}
    Value(string vs) : T(VSTRING), Btemp(false), Itemp(0), Rtemp(0.0), Stemp(vs) {}
    
    
    ValType GetType() const { return T; }
    bool IsErr() const { return T == VERR; }
    bool IsInt() const { return T == VINT; }
    bool IsString() const { return T == VSTRING; }
    bool IsReal() const {return T == VREAL;}
    bool IsBool() const {return T == VBOOL;}
    
    int GetInt() const { if( IsInt() ) return Itemp; throw "RUNTIME ERROR: Value not an integer"; }
    
    string GetString() const { if( IsString() ) return Stemp; throw "RUNTIME ERROR: Value not a string"; }
    
    float GetReal() const { if( IsReal() ) return Rtemp; throw "RUNTIME ERROR: Value not an integer"; }
    
    bool GetBool() const {if(IsBool()) return Btemp; throw "RUNTIME ERROR: Value not a boolean";}
    
    void SetType(ValType type)
    {
    	T = type;
	}
	
    void SetInt(int val)
    {
    	Itemp = val;
	}
	
	void SetReal(float val)
    {
    	Rtemp = val;
	}
	
	void SetString(string val)
    {
    	Stemp = val;
	}
	
	void SetBool(bool val)
    {
    	Btemp = val;
	}
	
	
    //add op to this
    Value operator+(const Value& op) const {
		Value err;
		if (this->IsInt()) {
			if (op.IsInt()) {
				Value v(this->GetInt() + op.GetInt());
				return v;
			}
			else if (op.IsReal()) {
				Value v(this->GetInt() + op.GetReal());
				return v;
			}
		}
		else if (this->IsReal()) {
			if (op.IsInt()) {
				Value v(this->GetReal() + op.GetInt());
				return v;
			}
			else if (op.IsReal()) {
				Value v(this->GetReal() + op.GetReal());
				return v;
			}

		}
		else if (this->IsString()) {
			if (op.IsString()) {
				Value v(this->GetString() + op.GetString());
				return v;
			}
		}
		
		return err;
	}
    
    // subtract op from this
    Value operator-(const Value& op) const {
		Value err;
		if (this->IsInt()) {
			if (op.IsInt()) {
				Value v(this->GetInt() - op.GetInt());
				return v;
			}
			else if (op.IsReal()) {
				Value v(this->GetInt() - op.GetReal());
				return v;
			}
		}
		else if (this->IsReal()) {
			if (op.IsInt()) {
				Value v(this->GetReal() - op.GetInt());
				return v;
			}
			else if (op.IsReal()) {
				Value v(this->GetReal() - op.GetReal());
				return v;
			}

		}
		return err;
	}
    // multiply this by op
    Value operator*(const Value& op) const {
		Value err;
		if (this->IsInt()) {
			if (op.IsInt()) {
				Value v(this->GetInt() * op.GetInt());
				return v;
			}
			else if (op.IsReal()) {
				Value v(this->GetInt() * op.GetReal());
				return v;
			}
		}
		else if (this->IsReal()) {
			if (op.IsInt()) {
				Value v(this->GetReal() * op.GetInt());
				return v;
			}
			else if (op.IsReal()) {
				Value v(this->GetReal() * op.GetReal());
				return v;
			}
		}
		
		return err;
	}
    
    
    // divide this by op
    Value operator/(const Value& op) const {
		Value err;
		if (this->IsInt()) {
			if (op.IsInt()) {
				Value v(this->GetInt() / op.GetInt());
				return v;
			}
			else if (op.IsReal()) {
				Value v(this->GetInt() / op.GetReal());
				return v;
			}
		}
		else if (this->IsReal()) {
			if (op.IsInt()) {
				Value v(this->GetReal() / op.GetInt());
				return v;
			}
			else if (op.IsReal()) {
				Value v(this->GetReal() / op.GetReal());
				return v;
			}
		}
		
		return err;
	}
    
    Value operator==(const Value& op) const {
		Value err;
		if ((this->GetType() == op.GetType()) && !(this->IsErr()) && !(op.IsErr())) {
			if (this->IsInt()) {
				Value v(this->GetInt() == op.GetInt());
				return v;
			}
			if (this->IsBool()) {
				Value v(this->GetBool() == op.GetBool());
				return v;
			}
			if (this->IsReal()) {
				Value v(this->GetReal() == op.GetReal());
				return v;
			}
			if (this->IsString()) {
				Value v(this->GetString() == op.GetString());
				return v;
			}
		}


		if (this->IsInt() && op.IsReal()) {
			Value v(this->GetInt() == op.GetReal());
			return v;
		}

		if (this->IsReal() && op.IsInt()) {
			Value v(this->GetReal() == op.GetInt());
			return v;
		}
		
		return err;
	}
    

	Value operator>(const Value& op) const {
		Value err;
		if ((this->GetType() == op.GetType()) && !(this->IsErr()) && !(op.IsErr())) {
			if (this->IsInt()) {
				Value v(this->GetInt() > op.GetInt());
				return v;
			}
			if (this->IsBool()) {
				Value v(this->GetBool() > op.GetBool());
				return v;
			}
			if (this->IsReal()) {
				Value v(this->GetReal() > op.GetReal());
				return v;
			}
			if (this->IsString()) {
				Value v(this->GetString() > op.GetString());
				return v;
			}
		}


		if (this->IsInt() && op.IsReal()) {
			Value v(this->GetInt() > op.GetReal());
			return v;
		}

		if (this->IsReal() && op.IsInt()) {
			Value v(this->GetReal() > op.GetInt());
			return v;
		}
		
		return err;
	}
	
	Value operator<(const Value& op) const {
		Value err;
		if ((this->GetType() == op.GetType()) && !(this->IsErr()) && !(op.IsErr())) {
			if (this->IsInt()) {
				Value v(this->GetInt() < op.GetInt());
				return v;
			}
			if (this->IsBool()) {
				Value v(this->GetBool() < op.GetBool());
				return v;
			}
			if (this->IsReal()) {
				Value v(this->GetReal() < op.GetReal());
				return v;
			}
			if (this->IsString()) {
				Value v(this->GetString() < op.GetString());
				return v;
			}
		}


		if (this->IsInt() && op.IsReal()) {
			Value v(this->GetInt() < op.GetReal());
			return v;
		}

		if (this->IsReal() && op.IsInt()) {
			Value v(this->GetReal() < op.GetInt());
			return v;
		}
		
		return err;
	}
	    
    friend ostream& operator<<(ostream& out, const Value& op) {
        if( op.IsInt() ) out << op.Itemp;
        else if( op.IsString() ) out << op.Stemp;
        else if( op.IsReal()) out <<  fixed << showpoint << setprecision(2) << op.Rtemp;
        else out << "ERROR";
        return out;
    }
};


#endif
