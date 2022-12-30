/*
    _____ ____  ____  ____
   |  ___/ ___||  _ \|  _ \  || C++17 filesystem binding for Lua
   | |_  \___ \| |_) | |_) | || https://github.com/UrNightmaree/filesystempp
   |  _|  ___) |  __/|  __/  ||
   |_|   |____/|_|   |_|     ||


   MIT License

   Copyright (c) 2022 Koosh

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.

*/

#include <bits/stdint-uintn.h>
#include <cstring>
#include <cstdarg>

#include <iostream>
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vector>

#define C extern "C"

namespace fs = std::filesystem;

typedef std::vector<const char*>* CCVector;

/* Misc */

C int getCCVectorSize(CCVector vec) {
	return vec->size();
}

C const char* getCCVectorValue(CCVector vec, int i) {
	return vec->at(i);
}

C void freeCCVector(CCVector vec) {
	delete vec;
}

/* Simple functions */

/** Maps **/

std::map<const char*, fs::copy_options> CopyOptions = {
	{"none", fs::copy_options::none}, 
	{"skip_existing", fs::copy_options::skip_existing}, 
	{"overwrite_existing", fs::copy_options::overwrite_existing}, 
	{"update_existing", fs::copy_options::update_existing}, 
	{"recursive", fs::copy_options::recursive}, 
	{"copy_symlinks", fs::copy_options::copy_symlinks}, 
	{"skip_symlinks", fs::copy_options::skip_symlinks}, 
	{"directories_only", fs::copy_options::directories_only}, 
	{"create_symlinks", fs::copy_options::create_symlinks}, 
	{"create_hard_links", fs::copy_options::create_hard_links}
};

std::map<const char*, fs::perm_options> PermissionOption = {
	{"replace", fs::perm_options::replace},
	{"add", fs::perm_options::add},
	fs::perm_options::remove},
	fs::perm_options::nofollow}
};

std::vector<fs::perms> Permissions = {
	fs::perms::none,
	fs::perms::owner_read,
	fs::perms::owner_write,
	fs::perms::owner_exec,
	fs::perms::owner_all,
	fs::perms::group_read,
	fs::perms::group_write,
	fs::perms::group_exec,
	fs::perms::group_all,
	fs::perms::others_read,
	fs::perms::others_write,
	fs::perms::others_exec,
	fs::perms::others_all,
	fs::perms::all,
	fs::perms::set_uid,
	fs::perms::set_gid,
	fs::perms::sticky_bit,
	fs::perms::mask
};

/** Maps End **/

C const char* FSPP_absolute(const char* path) {
	std::string abs_path = fs::absolute(path).string();

	char* c_abs_path = new char[abs_path.length() + 1];
	strcpy(c_abs_path, abs_path.c_str());
	return c_abs_path;
}

C void FSPP_copy(const char* from, const char* to, int n, ...) {
	fs::copy_options co;

	va_list args;
	va_start(args, n);

	for (int i = 0; i < n; i++) {
		const char* opt = va_arg(args, const char*);

		co |= CopyOptions.at(opt);
	}

	va_end(args);

	if (n == 0) fs::copy(from, to); else fs::copy(from, to, co);
}

C int FSPP_copyfile(const char* from, const char* to, int n, ...) {
	fs::copy_options co;

	va_list args;
	va_start(args, n);

	for (int i = 0; i < n; i++) {
		const char* opt = va_arg(args, const char*);

		co |= CopyOptions.at(opt);
	}

	va_end(args);

	return n == 0 ? fs::copy_file(from, to) : fs::copy_file(from, to, co);
}

C void FSPP_copysymlink(const char* from, const char* to) {
	fs::copy_symlink(from, to);
}

C int FSPP_createdirectory(const char* path) {
	return fs::create_directory(path);
}

C int FSPP_arg2_createdirectory(const char* path, const char* existing_path) {
	return fs::create_directory(path, existing_path);
}

C int FSPP_createdirectories(const char* path) {
	return fs::create_directories(path);
}

C void FSPP_createhardlink(const char* target, const char* link) {
	fs::create_hard_link(target, link);
}

C void FSPP_createsymlink(const char* target, const char* link) {
	fs::create_symlink(target, link);
}

C void FSPP_createdirectorysymlink(const char* target, const char* link) {
	fs::create_directory_symlink(target, link);
}

C const char* FSPP_currentpath() {
	std::string curr_path = fs::current_path().string();

	char* path = new char[curr_path.length() + 1];
	strcpy(path, curr_path.c_str());
	return (const char*) path;
}

C void FSPP_change_currentpath(const char* path) {
	fs::current_path(path);
}

C int FSPP_exists(const char* path) {
	return fs::exists(path);
}

C int FSPP_equivalent(const char* path1, const char* path2) {
	return fs::equivalent(path1, path2);
}

C uint64_t FSPP_filesize(const char* path) {
	return (uint64_t) fs::file_size(path);
}

C uint64_t FSPP_hardlinkcount(const char* path) {
	return (uint64_t) fs::hard_link_count(path);
}

C void FSPP_permissions(const char* path, int opt, int n, ...) {
	fs::perms perms = fs::perms::none;
	fs::perm_options perm_opt = PermissionOption.at(opt - 1);
	std::cout << "debug" << '\n';

	va_list args;
	va_start(args, n);

	for (int i = 0; i < n; i++) {
		int perm = (int) va_arg(args, int);

		std::cout << i << ' ' << perm - 1 << '\n';
		perms |= Permissions.at(perm - 1);
	}

	va_end(args);

	fs::permissions(path, perms, perm_opt);
}

C const char* FSPP_readsymlink(const char* path) {
	std::string orig_path = fs::read_symlink(path).string();

	char* c_orig_path = new char[orig_path.length() + 1];
	strcpy(c_orig_path, orig_path.c_str());
	return c_orig_path;
}

C CCVector FSPP_directoryiterator(const char* Cpath) {
	fs::path path(Cpath);

	CCVector vec = new std::vector<const char*>;

	for (auto const& entry : fs::directory_iterator{path}) {
		std::string entry_path = entry.path().string();

		char* content = new char[entry_path.length() + 1];
		strcpy(content, entry_path.c_str());
		vec->push_back(content);
	}

	return vec;
}

/* For Debugging */

#ifdef _DEBUG
int main() {
	FSPP_permissions("smth","add",3,"owner_exec","group_exec","others_exec");
}
#endif /* _DEBUG */
