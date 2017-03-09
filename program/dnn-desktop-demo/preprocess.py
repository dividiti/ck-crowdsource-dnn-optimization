import sys
import os

APP_CONF_FILE = 'app.conf'

def ck_preprocess(i):
    if sys.version_info[0]>2:
        import configparser as cp
    else:
        import ConfigParser as cp

    # read existing desktop app config, if any
    conf = cp.ConfigParser()
    conf.read(APP_CONF_FILE)

    ck = i['ck_kernel']

    r = fill_general(ck, conf)
    if r['return'] > 0: return r

    r = fill_models(ck, conf)
    if r['return'] > 0: return r

    exe_extension = ck.get_by_flat_key({'dict': i, 'key': '##host_os_dict#file_extensions#exe'}).get('value', '')

    r = fill_programs(ck, conf, exe_extension)
    if r['return'] > 0: return r

    r = fill_aux(ck, conf)
    if r['return'] > 0: return r

    r = fill_val(ck, conf)
    if r['return'] > 0: return r

    with open(APP_CONF_FILE, 'wb') as f:
        conf.write(f)

    return {'return':0, 'bat':'', 'new_env': i['env']}

def setstr(conf, section, key, value):
    # string values must be enquoted for Qt to read them correctly
    conf.set(section, key, '"' + value.replace('\\', '\\\\') + '"')

def ensure_section(conf, section, clean=False):
    if clean:
        conf.remove_section(section)
    if not conf.has_section(section):
        conf.add_section(section)

def fill_general(ck, conf):
    ensure_section(conf, 'General')
    try:
        bin_path, bin_name = os.path.split(which('ck'))
        setstr(conf, 'General', 'ck_bin_path', bin_path)
        setstr(conf, 'General', 'ck_exe_name', bin_name)
    except WhichError:
        return {'return':1, 'error': 'Path to ck not found'}

    r = ck.access({'action': 'where', 'module_uoa': 'repo', 'data_uoa': 'local'})
    if r['return'] > 0: return r

    setstr(conf, 'General', 'ck_repos_path', os.path.dirname(r['path']))
    return {'return':0}

def fill_section(ck, conf, section, tags, module=''):
    ensure_section(conf, section, True)
    search_dict = {'action': 'search', 'tags': tags}
    if module != '':
        search_dict['module_uoa'] = module
    r = ck.access(search_dict)
    if r['return'] > 0: return r
    
    lst = r['lst']
    conf.set(section, 'count', len(lst))

    for i, u in enumerate(lst):
        module_uoa = u['module_uoa']
        data_uoa = u['data_uoa']
        r = ck.access({'action': 'load', 'module_uoa': module_uoa, 'data_uoa': data_uoa})
        if r['return'] > 0: return r
        u['meta'] = r['dict']
        setstr(conf, section, str(i) + '_uoa', data_uoa)
        setstr(conf, section, str(i) + '_name', r.get('data_name', u))

    return {'return':0, 'lst': lst}

def fill_models(ck, conf):
    return fill_section(ck, conf, section='Models', tags='caffemodel', module='env')

def fill_programs(ck, conf, exe_extension):
    section = 'Programs'
    r = fill_section(ck, conf, section=section, tags='caffe-classification,continuous')
    if r['return'] > 0: return r
    lst = r['lst']
    for i, u in enumerate(lst):
        output_file = ck.get_by_flat_key({'dict': u, 'key': '##meta#run_cmds#use_continuous#run_time#run_cmd_out1'}).get('value', None)
        if None == output_file:
            return {'return': 1, 'error': 'Could not find output file for ' + u['data_uoa']}
        r = ck.access(['find', '--module_uoa=' + u['module_uoa'], '--data_uoa=' + u['data_uoa']])
        if r['return'] > 0: return r
        output_file = os.path.join(r['path'], 'tmp', output_file)
        setstr(conf, section, str(i) + '_output_file', output_file)

        target_file = ck.get_by_flat_key({'dict': u, 'key': '##meta#target_file'}).get('value', None)
        if None == target_file:
            return {'return': 1, 'error': 'Could not find target file for ' + u['data_uoa']}
        if not target_file.startswith(exe_extension):
            target_file = target_file + exe_extension
        setstr(conf, section, str(i) + '_exe', target_file)

    return {'return': 0}

def fill_aux(ck, conf):
    return fill_section(ck, conf, section='AUX', tags='imagenet,aux', module='env')

def fill_val(ck, conf):
    return fill_section(ck, conf, section='VAL', tags='imagenet,val', module='env')

#
# =============================================================================
#

# Copyright (c) 2002-2007 ActiveState Software Inc.
# Author:
#   Trent Mick (TrentM@ActiveState.com)
# Home:
#   http://trentm.com/projects/which/
# 
# LICENSE: MIT
# 
# Copyright (c) 2002-2005 ActiveState Corp.
# 
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
# 
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
# 

import getopt
import stat

#---- exceptions

class WhichError(Exception):
    pass

#---- internal support stuff

def _getRegisteredExecutable(exeName):
    """Windows allow application paths to be registered in the registry."""
    registered = None
    if sys.platform.startswith('win'):
        if os.path.splitext(exeName)[1].lower() != '.exe':
            exeName += '.exe'
        import _winreg
        try:
            key = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\" +\
                  exeName
            value = _winreg.QueryValue(_winreg.HKEY_LOCAL_MACHINE, key)
            registered = (value, "from HKLM\\"+key)
        except _winreg.error:
            pass
        if registered and not os.path.exists(registered[0]):
            registered = None
    return registered

def _samefile(fname1, fname2):
    if sys.platform.startswith('win'):
        return ( os.path.normpath(os.path.normcase(fname1)) ==\
            os.path.normpath(os.path.normcase(fname2)) )
    else:
        return os.path.samefile(fname1, fname2)

def _cull(potential, matches, verbose=0):
    """Cull inappropriate matches. Possible reasons:
        - a duplicate of a previous match
        - not a disk file
        - not executable (non-Windows)
    If 'potential' is approved it is returned and added to 'matches'.
    Otherwise, None is returned.
    """
    for match in matches:  # don't yield duplicates
        if _samefile(potential[0], match[0]):
            if verbose:
                sys.stderr.write("duplicate: %s (%s)\n" % potential)
            return None
    else:
        if not stat.S_ISREG(os.stat(potential[0]).st_mode):
            if verbose:
                sys.stderr.write("not a regular file: %s (%s)\n" % potential)
        elif sys.platform != "win32" \
             and not os.access(potential[0], os.X_OK):
            if verbose:
                sys.stderr.write("no executable access: %s (%s)\n"\
                                 % potential)
        else:
            matches.append(potential)
            return potential

#---- module API

def whichgen(command, path=None, verbose=0, exts=None):
    """Return a generator of full paths to the given command.
    
    "command" is a the name of the executable to search for.
    "path" is an optional alternate path list to search. The default it
        to use the PATH environment variable.
    "verbose", if true, will cause a 2-tuple to be returned for each
        match. The second element is a textual description of where the
        match was found.
    "exts" optionally allows one to specify a list of extensions to use
        instead of the standard list for this system. This can
        effectively be used as an optimization to, for example, avoid
        stat's of "foo.vbs" when searching for "foo" and you know it is
        not a VisualBasic script but ".vbs" is on PATHEXT. This option
        is only supported on Windows.

    This method returns a generator which yields either full paths to
    the given command or, if verbose, tuples of the form (<path to
    command>, <where path found>).
    """
    matches = []
    if path is None:
        usingGivenPath = 0
        path = os.environ.get("PATH", "").split(os.pathsep)
        if sys.platform.startswith("win"):
            path.insert(0, os.curdir)  # implied by Windows shell
    else:
        usingGivenPath = 1

    # Windows has the concept of a list of extensions (PATHEXT env var).
    if sys.platform.startswith("win"):
        if exts is None:
            exts = os.environ.get("PATHEXT", "").split(os.pathsep)
            # If '.exe' is not in exts then obviously this is Win9x and
            # or a bogus PATHEXT, then use a reasonable default.
            for ext in exts:
                if ext.lower() == ".exe":
                    break
            else:
                exts = ['.COM', '.EXE', '.BAT']
        elif not isinstance(exts, list):
            raise TypeError("'exts' argument must be a list or None")
    else:
        if exts is not None:
            raise WhichError("'exts' argument is not supported on "\
                             "platform '%s'" % sys.platform)
        exts = []

    # File name cannot have path separators because PATH lookup does not
    # work that way.
    if os.sep in command or os.altsep and os.altsep in command:
        if os.path.exists(command):
            match = _cull((command, "explicit path given"), matches, verbose)
            if verbose:
                yield match
            else:
                yield match[0]
    else:
        for i in range(len(path)):
            dirName = path[i]
            # On windows the dirName *could* be quoted, drop the quotes
            if sys.platform.startswith("win") and len(dirName) >= 2\
               and dirName[0] == '"' and dirName[-1] == '"':
                dirName = dirName[1:-1]
            for ext in ['']+exts:
                absName = os.path.abspath(
                    os.path.normpath(os.path.join(dirName, command+ext)))
                if os.path.isfile(absName):
                    if usingGivenPath:
                        fromWhere = "from given path element %d" % i
                    elif not sys.platform.startswith("win"):
                        fromWhere = "from PATH element %d" % i
                    elif i == 0:
                        fromWhere = "from current directory"
                    else:
                        fromWhere = "from PATH element %d" % (i-1)
                    match = _cull((absName, fromWhere), matches, verbose)
                    if match:
                        if verbose:
                            yield match
                        else:
                            yield match[0]
        match = _getRegisteredExecutable(command)
        if match is not None:
            match = _cull(match, matches, verbose)
            if match:
                if verbose:
                    yield match
                else:
                    yield match[0]


def which(command, path=None, verbose=0, exts=None):
    """Return the full path to the first match of the given command on
    the path.
    
    "command" is a the name of the executable to search for.
    "path" is an optional alternate path list to search. The default it
        to use the PATH environment variable.
    "verbose", if true, will cause a 2-tuple to be returned. The second
        element is a textual description of where the match was found.
    "exts" optionally allows one to specify a list of extensions to use
        instead of the standard list for this system. This can
        effectively be used as an optimization to, for example, avoid
        stat's of "foo.vbs" when searching for "foo" and you know it is
        not a VisualBasic script but ".vbs" is on PATHEXT. This option
        is only supported on Windows.

    If no match is found for the command, a WhichError is raised.
    """
    try:
        match = whichgen(command, path, verbose, exts).next()
    except StopIteration:
        raise WhichError("Could not find '%s' on the path." % command)
    return match


def whichall(command, path=None, verbose=0, exts=None):
    """Return a list of full paths to all matches of the given command
    on the path.  

    "command" is a the name of the executable to search for.
    "path" is an optional alternate path list to search. The default it
        to use the PATH environment variable.
    "verbose", if true, will cause a 2-tuple to be returned for each
        match. The second element is a textual description of where the
        match was found.
    "exts" optionally allows one to specify a list of extensions to use
        instead of the standard list for this system. This can
        effectively be used as an optimization to, for example, avoid
        stat's of "foo.vbs" when searching for "foo" and you know it is
        not a VisualBasic script but ".vbs" is on PATHEXT. This option
        is only supported on Windows.
    """
    return list( whichgen(command, path, verbose, exts) )
