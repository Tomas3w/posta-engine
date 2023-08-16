import re

original_string = "This is a sample sentence with a word::hola and another word."
word_to_replace = "word"
replacement_word = "replacement"

# Use regular expression to replace whole words only
new_string = re.sub(r'\b' + re.escape(word_to_replace) + r'\b', replacement_word, original_string)

print(new_string)
