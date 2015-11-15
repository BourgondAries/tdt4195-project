for i=1:3
	image = imread(['image' num2str(i) '.png']);

	% Get the red checkers
	% RGB -> Grey
	gray = @(matrix) matrix(:, :, 1) * 0.2126 + ...
		matrix(:, :, 2) * 0.7152 + matrix(:, :, 3) * 0.0722;
	image = gray(image);
	imwrite(image, ['image' num2str(i) '-filter0.png']);

	% Now use some morphological closing
	circle = strel('disk', 25);
	image = imopen(image, circle);
	imwrite(image, ['image' num2str(i) '-filter1.png']);

	% Select all color values below a certain treshold
	avg = mean(mean(image));
	image = image < avg * 0.6;
	imwrite(image, ['image' num2str(i) '-filter2.png']);

	% Remove any small objects using opening
	image = imerode(image, circle);
	imwrite(image, ['image' num2str(i) '-filter3.png']);

	% Remove the edges, because they may be shaded wrongly.
	pixels = pruneEdges(image);
	image = grow(image, pixels);

	imwrite(image, ['image' num2str(i) '-processed.png']);
end
