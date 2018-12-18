from invoke import run, config, task, Collection
import os
from subprocess import check_call, call
import contextlib
import shutil
import webbrowser

BUILD_PATH = 'build'
PROJECT_NAME = 'li_can_slv'

on_rtd = os.environ.get('READTHEDOCS', None) == 'True'


@task
def env(ctx):
    os.environ["PATH"] = os.pathsep.join(ctx.path.values()) + os.pathsep + os.environ["PATH"]


@task
def env_gcc(ctx):
    os.environ["PATH"] = os.pathsep.join([ctx.path['mingw32']]) + os.pathsep + os.environ["PATH"]


@task(env_gcc, aliases=['cmakeGenerateMakeFileMINGW'])
def configure(ctx):
    shutil.rmtree(BUILD_PATH, ignore_errors=True)
    os.mkdir(BUILD_PATH)
    with ctx.cd(BUILD_PATH):
        ctx.run('cmake -DCMAKE_TOOLCHAIN_FILE=tools/toolchain-tdm_gcc32.cmake -G"Ninja" ..')


@task(env)
def cov(ctx):
    shutil.rmtree(BUILD_PATH + '/coverage', ignore_errors=True)
    os.mkdir(BUILD_PATH + '/coverage')
    with ctx.cd(BUILD_PATH):
        ctx.run('gcovr -r . --html --html-details -k -o ./coverage/coverage-details.html')

    shutil.rmtree('.//verify//artifacts', ignore_errors=True)
    shutil.move(BUILD_PATH + '/coverage', './/verify//artifacts//coverage')


@task(env)
def all(ctx):
    with ctx.cd(BUILD_PATH):
        ctx.run('"{}" -j8 all'.format(ctx.make.ninja_make))


@task(all)
def make(ctx):
    pass


@task(env)
def clean(ctx):
    with ctx.cd(BUILD_PATH):
        ctx.run('"{}" clean'.format(ctx.make.ninja_make))


@task
def clean_all(ctx):
    shutil.rmtree(BUILD_PATH, ignore_errors=True)
    shutil.rmtree('docs/sphinx/doxyxml', ignore_errors=True)
    shutil.rmtree('docs/sphinx/doxyhtml', ignore_errors=True)


@task(env, aliases=['unittestWorkstationRunMinGWGcc'])
def test_run(ctx):
    with ctx.cd(BUILD_PATH):
        ctx.run('"{}" unittest_run'.format(ctx.make.ninja_make))


@task(env)
def test_junit(ctx):
    ctx.run('python .\\verify\\unity\\auto\\unity_to_junit.py "{}"'.format('build//verify//'))
    filename = 'result.xml'
    shutil.move(os.path.join('.', filename), os.path.join('verify//artifacts', filename))


@task(env, aliases=['unittestGenerateTestRunner'])
def test_conf(ctx):
    with ctx.cd(BUILD_PATH):
        ctx.run('"{}" unittest_prepare'.format(ctx.make.ninja_make))


@task(test_run, test_junit)
def test(ctx):
    pass


@task
def diff(ctx):
    ctx.run("git difftool --dir-diff")


@task
def doxy(ctx):

    shutil.rmtree('docs\\sphinx\\doxyxml', ignore_errors=True)
    shutil.rmtree('docs\\sphinx\\doxyhtml', ignore_errors=True)

    with ctx.cd('docs\\sphinx'):
        ctx.run('doxygen.exe Doxyfile')
        
#    shutil.copytree(path_prefix + 'build\\docs\\doxy\\xml', path_prefix + 'docs\\sphinx\\_doxy\\xml')


@task
def breathe(ctx):
    # do('docs/sphinx', "make html")
    # print os.path.join(os.getcwd(), 'build/docs/doxy/xml')
    version = r'master'
    # docs_dir = os.path.dirname(os.path.realpath(__file__))
    # print docs_dir
    source_dir = os.path.join(os.getcwd(), 'docs/sphinx')
    out_dir = os.path.join(os.getcwd(), 'build/docs/sphinx/html')

    # shutil.rmtree('docs/sphinx/_doxy/xml', ignore_errors=True)
    shutil.rmtree('docs/sphinx/doxyxml', ignore_errors=True)

    check_call(['sphinx-build',
                '-Dversion=' + version, '-Drelease=' + version,
                '-Aversion=' + version,
                '-b', 'html', source_dir, out_dir])


@task(breathe)
def doc(ctx):
    pass


@task
def show_doc(ctx):
    out_dir = os.path.join(os.getcwd(), 'build/docs/sphinx/html')
    webbrowser.open(out_dir + "\\index.html")
