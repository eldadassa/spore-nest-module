mkdir -p style-reports
now=$(date +"%m-%d-%Y-%T")
flake8 --ignore=E241,F403,F405 --max-line-length=120 --exclude style-reports/ --tee --output-file style-reports/report-${now}.txt
# echo $?
# E501 line too long (89 > 79 characters)
# W293 blank line contains whitespace
# W291 trailing whitespace
# E265 block comment should start with '# '
# E231 missing whitespace after ':'
# E127 continuation line over-indented for visual indent
# E262 inline comment should start with '# '
# E241 multiple spaces after ','
# F403 star-imports
# F405 variable undefined or from star-import
exit $?
