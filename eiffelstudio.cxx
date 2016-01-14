#include <iostream>
#include "swigmod.h"


class EIFFELSTUDIO : public Language {
private:

	void add_return_type(Hash *a_methods_values, SwigType *a_type);
	void add_arguments(ParmList *a_values, ParmList *a_parms);
	Hash * get_type_value(SwigType *a_type);

protected:
   File *f_module;
   File *f_not_used;
   Hash *h_type_map;
public:

   virtual void main(int argc, char *argv[]);

   virtual int top(Node *n);

   virtual int functionWrapper(Node *n);

   virtual int constantWrapper(Node *n);

   virtual int membervariableHandler(Node *n);
   
   virtual int constructorHandler(Node *);
   
   virtual int destructorHandler(Node *);

   virtual int globalvariableHandler(Node *n);

};

extern "C" Language *
swig_eiffelstudio(void) {
  return new EIFFELSTUDIO();
}

Hash * create_type_map(){
	Hash * map = NewHash();
	Setattr(map, "*", "POINTER");
	Setattr(map, "int", "INTEGER");
	Setattr(map, "signed", "INTEGER");
	Setattr(map, "signed int", "INTEGER");
	Setattr(map, "unsigned int", "NATURAL");
	Setattr(map, "unsigned", "NATURAL");
	Setattr(map, "char", "INTEGER_8");
	Setattr(map, "signed char", "INTEGER_8");
	Setattr(map, "unsigned char", "NATURAL_8");
	Setattr(map, "short", "INTEGER_16");
	Setattr(map, "signed short", "INTEGER_16");
	Setattr(map, "unsigned short", "NATURAL_16");
	Setattr(map, "long", "INTEGER_32");
	Setattr(map, "signed long", "INTEGER_32");
	Setattr(map, "unsigned long", "NATURAL_32");
	Setattr(map, "long long", "INTEGER_64");
	Setattr(map, "signed long long", "INTEGER_64");
	Setattr(map, "unsigned long long", "NATURAL_64");
	Setattr(map, "bool", "BOOLEAN");
	Setattr(map, "float", "REAL_32");
	Setattr(map, "double", "REAL_64");
	return map;
}

void EIFFELSTUDIO::main(int argc, char *argv[]) {
   SWIG_library_directory("eiffelstudio");
   Preprocessor_define("EIFFELSTUDIO 1", 0);
   SWIG_config_file("eiffelstudio.swg");
   SWIG_typemap_lang("eiffelstudio");
}


void Eiffel_header(File * a_file, String * a_name) {
   Printf(a_file, "note\n");
   Printf(a_file, "\tdescription: \"Module {%s}\"\n", a_name);
   Printf(a_file, "\tgenerator: \"SWIG EiffelStudio module\"\n");
   Printf(a_file, "\tdate: \"$Date$\"\n");
   Printf(a_file, "\trevision: \"$Revision$\"\n");
   Printf(a_file, "\n\nclass\n\t%s\n", a_name);
   Printf(a_file, "\n\nfeature -- C externals\n\n");
}


void Eiffel_footer(File * a_file) {
   Printf(a_file, "end\n");
}



int EIFFELSTUDIO::top(Node *n) {

   /* Get the module name */
   String *module = Swig_string_upper(Getattr(n,"name"));

   /* Get the output file name */
   String *outfile = Getattr(n,"outfile");

   /* Initialize I/O (see next section) */
   String *filen = NewStringf("%s%s.e", SWIG_output_directory(), Swig_string_lower(module));
   f_module = NewFile(filen, "w", SWIG_output_files());
   if (!f_module) {
      FileErrorDisplay(Swig_string_lower(module));
      SWIG_exit(EXIT_FAILURE);
   }

   f_not_used = NewString("");

   h_type_map = create_type_map();

   Swig_register_filebyname("header", f_module);
   Swig_register_filebyname("wrapper", f_module);
   Swig_register_filebyname("runtime", f_not_used);

   /* Output module initialization code */
//   Swig_banner(f_begin);

//   Eiffel_header(f_module, module);
   
   /* Emit code for children */
   Language::top(n);

   /* Cleanup files */
   Delete(f_module);
   Delete(f_not_used);

   Delete(h_type_map);

   return SWIG_OK;
}

Hash *EIFFELSTUDIO::get_type_value(SwigType *a_type){
	String *Ctypestr = NULL;
	String *Etypestr = NULL;
	String * tempstr;
	SwigType *resolve_type = SwigType_typedef_resolve_all(a_type);
	Hash *result = NewHash();
	if(SwigType_ispointer(resolve_type) or SwigType_isarray(resolve_type)){
	   	Ctypestr = NewString(SwigType_str(SwigType_ltype(a_type), NULL));
		Etypestr = NewString(Getattr(h_type_map,"*"));
	} else if(SwigType_issimple(a_type)) {
		Ctypestr = NewString(SwigType_str(a_type, NULL));
		if (Strcmp(Ctypestr, "void")) {
			tempstr = Getattr(h_type_map,SwigType_str(resolve_type, NULL));
			if (tempstr) {
				Etypestr = NewString(tempstr);
			} else {
				Etypestr = NewString("UNKNOWN");
			}
		} else {
			Delete(Ctypestr);
			Ctypestr = NULL;
		}
	}
	if(Ctypestr and Etypestr){
		Replace(Ctypestr, " const", "", DOH_REPLACE_ANY);
		Replace(Ctypestr, "const ", "", DOH_REPLACE_ANY);
		Replace(Ctypestr, "const", "", DOH_REPLACE_ANY);
		Setattr(result, "Ctype", Ctypestr);
		Setattr(result, "Etype", Etypestr);
		Delete(Ctypestr);
		Delete(Etypestr);
	}
	return result;
}

void EIFFELSTUDIO::add_return_type(Hash *a_methods_values, SwigType *a_type){
	Hash * l_types = get_type_value(a_type);
	String * tempstr;
	if (Getattr(l_types, "Ctype") && Getattr(l_types, "Etype")) {
		tempstr = NewStringf(":%s", Getattr(l_types, "Ctype"));
		Setattr(a_methods_values, "Creturn", tempstr);
		Delete(tempstr);
		tempstr = NewStringf(":%s", Getattr(l_types, "Etype"));
		Setattr(a_methods_values, "Ereturn", tempstr);
		Delete(tempstr);
	} else {
		tempstr = NewString("");
		Setattr(a_methods_values, "Creturn", tempstr);
		Setattr(a_methods_values, "Ereturn", tempstr);
		Delete(tempstr);
	}
	Delete(l_types);
}

void EIFFELSTUDIO::add_arguments(ParmList *a_values, ParmList *a_parms) {
	SwigType *type;
	Hash * l_types = NULL;
	String *name;
	String *value;
	String *eArguments = NULL;
	String *eCparams = NULL;
	String * tempstr;
	Parm *p;
	for(p = a_parms; p; p = nextSibling(p)){
		if(p){
			type  = Getattr(p,"type");
			name  = Getattr(p,"name");
			value = Getattr(p,"value");
			l_types = get_type_value(type);
			if (!eArguments) {
				eArguments = NewStringf("(a_%s: %s", name, Getattr(l_types, "Etype"));
				eCparams = NewStringf("(%s", Getattr(l_types, "Ctype"));
			} else {
				tempstr = NewStringf("; a_%s: %s", name, Getattr(l_types, "Etype"));
				Append(eArguments, tempstr);
				Delete(tempstr);
				tempstr = NewStringf(",%s", Getattr(l_types, "Ctype"));
				Append(eCparams, tempstr);
				Delete(tempstr);
			}
			Delete(l_types);
			l_types = NULL;
		}
	}
	if (eArguments && eCparams) {
		Append(eArguments, ")");
		Append(eCparams, ")");
		Setattr(a_values, "Cparm", eCparams);
		Setattr(a_values, "Eparm", eArguments);
		Delete(eArguments);
		Delete(eCparams);
	} else {
		tempstr = NewString("");
		Setattr(a_values, "Cparm", tempstr);
		Setattr(a_values, "Eparm", tempstr);
		Delete(tempstr);
	}
}

int EIFFELSTUDIO::functionWrapper(Node *n) {
  /* Get some useful attributes of this function */
	String   *name   = Getattr(n,"sym:name");
	SwigType *type   = Getattr(n,"type");
	ParmList *parms  = Getattr(n,"parms");
	String *header = Getattr(n,"feature:h_file");
	String *prefix = Getattr(n,"feature:prefix");
	String *sufix = Getattr(n,"feature:sufix");

	Hash *l_values = NewHash();
	   
	String   *Cheader;
	add_return_type(l_values, type);
	add_arguments(l_values, parms);
	if (header) {
		Cheader = NewStringf(" | <%s>", header);
	} else {
		Cheader = NewString("");
	}
	Printf(f_module, "\tfrozen %s%s%s%s%s\n", prefix, name, sufix, Getattr(l_values, "Eparm"), Getattr(l_values, "Ereturn"));
	Printf(f_module, "\t\texternal\n");
	Printf(f_module, "\t\t\t\"C%s%s%s\"\n", Getattr(l_values, "Cparm"), Getattr(l_values, "Creturn"), Cheader);
	Printf(f_module, "\t\talias\n");
	Printf(f_module, "\t\t\t\"%s\"\n", name);
	Printf(f_module, "\t\tend\n\n");
	Delete(Cheader);
	Delete(l_values);
	return SWIG_OK;
}

int EIFFELSTUDIO::constantWrapper(Node *n) {
	String   *name   = Getattr(n,"sym:name");
	SwigType *type   = Getattr(n,"type");
	String *header = Getattr(n,"feature:h_file");
	String *prefix = Getattr(n,"feature:prefix");
	String *sufix = Getattr(n,"feature:sufix");
	String *numeric_type = Getattr(n,"feature:numeric_define_type");
	if ((numeric_type) && (!Strcmp(SwigType_str(type, NULL), "int"))) {
		type = numeric_type;
	}
	String   *Cheader;
	Hash *l_types = get_type_value(type);
	int result = SWIG_OK;
	if (header) {
		Cheader = NewStringf(" <%s>", header);
	} else {
		Cheader = NewString("");
	}
	if (Getattr(l_types, "Etype") && Getattr(l_types, "Ctype")) {
		Printf(f_module, "\tfrozen %s%s%s : %s\n", prefix, name, sufix, Getattr(l_types, "Etype"));
		Printf(f_module, "\t\texternal\n");
		Printf(f_module, "\t\t\t\"C [macro%s] : %s\"\n", Cheader, Getattr(l_types, "Ctype"));
		Printf(f_module, "\t\talias\n");
		Printf(f_module, "\t\t\t\"%s\"\n", name);
		Printf(f_module, "\t\tend\n\n");
	} else {
		Printf(stdout, "Cannot get Eiffel Type from C type %s.", SwigType_str(type, NULL));
		result = SWIG_ERROR;
	}
	Delete(Cheader);
	Delete(l_types);
	return result;
}

int EIFFELSTUDIO::membervariableHandler(Node *n){
	String *name   = Getattr(n,"sym:name");
	SwigType *type   = Getattr(n,"type");
	String *class_prefix = getClassPrefix();
	String *header = Getattr(n,"feature:h_file");
	String *prefix = Getattr(n,"feature:prefix");
	String *sufix = Getattr(n,"feature:sufix");
	String *mrename_get;
	String *mrename_set;
	String *mname;
	String *Cheader;
	Hash *l_types = get_type_value(type);
	int result = SWIG_OK;
	if (header) {
		Cheader = NewStringf(" <%s>", header);
	} else {
		Cheader = NewString("");
	}
	if (Getattr(l_types, "Etype") && Getattr(l_types, "Ctype")) {
		mname = Swig_name_member(0, class_prefix, name);
		mrename_get = Swig_name_get(getNSpace(), mname);
		Printf(f_module, "\tfrozen %s%s%s(a_pointer:POINTER) : %s\n", prefix, mrename_get, sufix, Getattr(l_types, "Etype"));
		Printf(f_module, "\t\texternal\n");
		Printf(f_module, "\t\t\t\"C [struct%s] (%s) : %s\"\n", Cheader, class_prefix, Getattr(l_types, "Ctype"));
		Printf(f_module, "\t\talias\n");
		Printf(f_module, "\t\t\t\"%s\"\n", name);
		Printf(f_module, "\t\tend\n\n");
		Delete(mrename_get);
		if (is_assignable(n)) {
			mrename_set = Swig_name_set(getNSpace(), mname);
			Printf(f_module, "\tfrozen %s%s%s(a_pointer:POINTER, a_%s:%s)\n", prefix, mrename_set, sufix, name, Getattr(l_types, "Etype"));
			Printf(f_module, "\t\texternal\n");
			Printf(f_module, "\t\t\t\"C [struct%s] (%s, %s)\"\n", Cheader, class_prefix, Getattr(l_types, "Ctype"));
			Printf(f_module, "\t\talias\n");
			Printf(f_module, "\t\t\t\"%s\"\n", name);
			Printf(f_module, "\t\tend\n\n");
			Delete(mrename_set);
		}
		Delete(mname);
	} else {
		Printf(stdout, "Cannot get Eiffel Type from C type %s.", SwigType_str(type, NULL));
		result = SWIG_ERROR;
	}
	Delete(Cheader);
	return result;
}

int EIFFELSTUDIO::constructorHandler(Node *n) {
	String *name   = Getattr(n,"sym:name");
	String *header = Getattr(n,"feature:h_file");
	String *prefix = Getattr(n,"feature:prefix");
	String *sufix = Getattr(n,"feature:sufix");
	String *mrename = Swig_name_construct(getNSpace(), name);
	String *Cheader;
	if (header) {
		Cheader = NewStringf(" use <%s>", header);
	} else {
		Cheader = NewString("");
	}
	Printf(f_module, "\tfrozen %s%s_size%s:INTEGER\n", prefix, name, sufix);
	Printf(f_module, "\t\texternal\n");
	Printf(f_module, "\t\t\t\"C inline%s\"\n", Cheader);
	Printf(f_module, "\t\talias\n");
	Printf(f_module, "\t\t\t\"sizeof(%s)\"\n", name);
	Printf(f_module, "\t\tend\n\n");
	Printf(f_module, "\tfrozen %s%s%s:POINTER\n", prefix, mrename, sufix);
	Printf(f_module, "\t\texternal\n");
	Printf(f_module, "\t\t\t\"C inline%s\"\n", Cheader);
	Printf(f_module, "\t\talias\n");
	Printf(f_module, "\t\t\t\"malloc(sizeof(%s))\"\n", name);
	Printf(f_module, "\t\tend\n\n");
	Delete(Cheader);

	return SWIG_OK;
}

int EIFFELSTUDIO::destructorHandler(Node *n) {
	String *name   = Getattr(n,"sym:name");
	String *header = Getattr(n,"feature:h_file");
	String *prefix = Getattr(n,"feature:prefix");
	String *sufix = Getattr(n,"feature:sufix");
	String *mrename;
	char *cname = Char(name);
	if (*cname == '~') {
		cname += 1;
	}
	mrename = Swig_name_destroy(getNSpace(), cname);
	String *Cheader;
	if (header) {
		Cheader = NewStringf(" use <%s>", header);
	} else {
		Cheader = NewString("");
	}
	Printf(f_module, "\tfrozen %s%s%s(self:POINTER)\n", prefix, mrename, sufix, cname);
	Printf(f_module, "\t\texternal\n");
	Printf(f_module, "\t\t\t\"C inline%s\"\n", Cheader);
	Printf(f_module, "\t\talias\n");
	Printf(f_module, "\t\t\t\"free($self)\"\n");
	Printf(f_module, "\t\tend\n\n");
	Delete(Cheader);
	return SWIG_OK;
}

int EIFFELSTUDIO::globalvariableHandler(Node *n) {
	String *name   = Getattr(n,"sym:name");
	SwigType *type   = Getattr(n,"type");
	String *header = Getattr(n,"feature:h_file");
	String *prefix = Getattr(n,"feature:prefix");
	String *sufix = Getattr(n,"feature:sufix");
	String *Cheader;
	Hash *l_types = get_type_value(type);
	int result = SWIG_OK;
	if (header) {
		Cheader = NewStringf(" use <%s>", header);
	} else {
		Cheader = NewString("");
	}
	if (Getattr(l_types, "Etype")) {
		Printf(f_module, "\tfrozen %s%s_get%s : %s\n", prefix, name, sufix, Getattr(l_types, "Etype"));
		Printf(f_module, "\t\texternal\n");
		Printf(f_module, "\t\t\t\"C inline%s\"\n", Cheader);
		Printf(f_module, "\t\talias\n");
		Printf(f_module, "\t\t\t\"%s\"\n", name);
		Printf(f_module, "\t\tend\n\n");
		Printf(f_module, "\tfrozen %s%s_set%s(a_value : %s)\n", prefix, name, sufix, Getattr(l_types, "Etype"));
		Printf(f_module, "\t\texternal\n");
		Printf(f_module, "\t\t\t\"C inline%s\"\n", Cheader);
		Printf(f_module, "\t\talias\n");
		Printf(f_module, "\t\t\t\"%s = $a_value\"\n", name);
		Printf(f_module, "\t\tend\n\n");
	} else {
		Printf(stdout, "Cannot get Eiffel Type from C type %s.", SwigType_str(type, NULL));
		result = SWIG_ERROR;
	}
	Delete(Cheader);
	return result;
}
