from invoke import run, config, task, Collection
import os
from subprocess import check_call, call
import contextlib
import shutil


BUILD_PATH = 'build'
PROJECT_NAME = 'li_can_slv'

on_rtd = os.environ.get('READTHEDOCS', None) == 'True'


@task
def env(ctx):
    os.environ["PATH"] = os.pathsep.join(ctx.path.values()) + os.pathsep + os.environ["PATH"]


@task
def env_gcc(ctx):
    os.environ["PATH"] = os.pathsep.join([ctx.path['mingw32'], ctx.path['cmake'],
                                          ctx.path['ninja']]) + os.pathsep + os.environ["PATH"]


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
        ctx.run('gcovr -r . --html --html-details -o ./coverage/coverage-details.html')


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
    shutil.rmtree('docs/sphinx/_doxyxml', ignore_errors=True)


@task(env, aliases=['unittestWorkstationRunMinGWGcc'])
def test_run(ctx):
    with ctx.cd(BUILD_PATH):
        ctx.run('"{}" unittest_run'.format(ctx.make.ninja_make))


@task(env)
def test_junit(ctx):
    ctx.run('python .\\verify\\unity\\auto\\unity_to_junit.py "{}"'.format('build//verify//source//'))
    filename = 'result.xml'
    shutil.move(os.path.join('.', filename), os.path.join(BUILD_PATH, filename))


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
    if on_rtd:
        print("invoke RTD build...")
        path_prefix = '.\\..\\..\\'
    else:
        print("invoke Normal build...")
        path_prefix = '.\\'

    shutil.rmtree(path_prefix + 'build\\docs\\doxy', ignore_errors=True)
    os.makedirs(path_prefix + 'build\\docs\\doxy')
    shutil.rmtree(path_prefix + 'docs\\sphinx\\_doxyxml', ignore_errors=True)

    with ctx.cd(path_prefix + 'docs\\doxy'):
        if on_rtd:
            ctx.run('doxygen li_can_slv.doxyfile')
        else:
            ctx.run('"{}" li_can_slv.doxyfile'.format(ctx.tools.doxygen))

    shutil.copytree(path_prefix + 'build\\docs\\doxy\\xml', path_prefix + 'docs\\sphinx\\_doxyxml')
    shutil.rmtree(path_prefix + 'build\\docs\\doxy\\xml', ignore_errors=True)


@task
def breathe(ctx):
    # do('docs/sphinx', "make html")
    # print os.path.join(os.getcwd(), 'build/docs/doxy/xml')
    version = r'master'
    # docs_dir = os.path.dirname(os.path.realpath(__file__))
    # print docs_dir
    source_dir = os.path.join(os.getcwd(), 'docs/sphinx')
    out_dir = os.path.join(os.getcwd(), 'build/docs/sphinx/html')

    shutil.rmtree('docs/sphinx/_doxy/xml', ignore_errors=True)

    check_call(['sphinx-build',
                '-Dversion=' + version, '-Drelease=' + version,
                '-Aversion=' + version,
                '-b', 'html', source_dir, out_dir])


@task(breathe)
def doc(ctx):
    pass
