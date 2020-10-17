# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'rugged/mariadb/version'

Gem::Specification.new do |spec|
  spec.name = 'rugged_mariadb'
  spec.version = Rugged::MariaDB::VERSION
  spec.authors = ['VANHECKE Mario']
  spec.email = ['mario@devchild.com']
  spec.summary = %q{A mariadb backend of rugged.}
  spec.description = %q{Enables rugged to store git objects and references into MariaDB.}
  spec.homepage = 'https://github.com/devchild/rugged_mariadb'
  spec.license = 'MIT'

  spec.files = `git ls-files -z`.split("\x0")
  spec.executables = spec.files.grep(%r{^bin/}) { |f| File.basename(f) }
  spec.test_files = spec.files.grep(%r{^(test|spec|features)/})
  spec.require_paths = ['lib']
  spec.extensions = %w[ext/rugged/mariadb/extconf.rb]

  spec.add_development_dependency 'bundler', '~> 1.7'
  spec.add_development_dependency 'rake', '~> 10.0'
  spec.add_development_dependency 'rake-compiler', '~> 0.9.3'
  spec.add_dependency 'rugged'


end
